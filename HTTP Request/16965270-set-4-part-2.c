#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define COM_BUF_LEN 512
#define BUF_LEN 200000

int main(int argc, char *argv[])
{
	int csd;	/* Client socket descriptor */
	struct sockaddr_in server;
	struct hostent *server_host;
	int string_size;
	short server_port;
	int send_request, inc_response;
	char response[COM_BUF_LEN], buf[BUF_LEN];
	char header[BUF_LEN];
	int writeStatus;
	int input;
	int rc, bc;	/* Buffer count and read count */
	int i;

	/* Correct commandline usage */
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s server\n", argv[0]);
		exit(EXIT_FAILURE);
	}


	server_host = gethostbyname(argv[1]);
	if (server_host == NULL)
	{
		herror("While calling gethostbyname()");
		exit(EXIT_FAILURE);
	}

	csd = socket(AF_INET, SOCK_STREAM, 0);
	if (csd < 0)
	{
		perror("While calling socket()");
		exit(EXIT_FAILURE);
	}

	/* Port 80 is port that servers listen to for web client */
	server_port = 80;
	server.sin_family = AF_INET;
	memcpy(&server.sin_addr.s_addr, server_host->h_addr_list[0],
	       server_host->h_length);
	server.sin_port = htons(server_port);

	if (connect(csd, (struct sockaddr*)&server, sizeof(server)) < 0)
	{
		perror("While calling connect()");
		exit(EXIT_FAILURE);
	}

	printf("Enter h for HEAD and g for GET request: ");
	input = getchar();
	if (input == 'h')
	{
		/* Create HEAD request */
		sprintf(header, "HEAD / HTTP/1.1\r\nHost: %s\r\n\r\n", argv[1]);
		printf("%s\n", header);

		/* Send request to server */
		writeStatus = write(csd, header, strlen(header));
		if (writeStatus < 0){
			perror("While writing request");
			exit(EXIT_FAILURE);
		} else if (writeStatus != strlen(header))
		{
			perror("Error occurred in writing but sent through anyways");
			printf("Request sent is %d size should be %d\n", 
			       writeStatus, (int)strlen(header));
		} 
		else
		{
			printf("Request sent\n");
		}

		/* Read response from server */
		/* HEAD method can not guarantee content-length key, host 
		   dependent
		  */
		if ((inc_response = read(csd, response, BUF_LEN)) < 0)
		{
			perror("While receiving response from server");
			exit(EXIT_FAILURE);
		}
		/* Null terminate end of response as buffer set as large value*/
		response[inc_response-1] = '\0';

		char *curLine = response;
		while(curLine)
		{
			/* Replace \n with \0 to prevent entire response being 
			   printed while searching for key-value pairs
			  */
			char *nextLine = strchr(curLine, '\n');
			if (nextLine) 
			{		
				*nextLine = '\0';
			}
			/* Parse response for Status */
			if (strstr(curLine,"HTTP") != NULL)
			{
				printf("%s\n",curLine);
			}
			/* Parse response for Content type */
			if (strstr(curLine,"Content-Type") != NULL)
			{
				printf("%s\n",curLine);
			}
			/* Parse response for Last-Modified*/	      
			if (strstr(curLine,"Last-Modified") != NULL)
			{
				printf("%s\n",curLine);
			}
			curLine = nextLine ? (nextLine+1) : NULL;
		}
	}
	else if (input == 'g')
	{
		/*building get request to webserver*/
		sprintf(header, "GET / HTTP/1.1\r\nHost: %s\r\n\r\n", 
		argv[1]);
		/*sending get request to webserver*/
		if ((writeStatus = write(csd, header, strlen(header))) < 0)
		{
			perror("While sending the request");
		}

		bc = 0;
		while(1)
		{
			rc = read(csd, response, COM_BUF_LEN);
			if (rc > 0)
			{
				if((bc + rc) > BUF_LEN)
				{
					fprintf(stderr, "Received buffer size exceeded!\n");
					close(csd);
					exit(EXIT_FAILURE);
				}
				memcpy(&buf[bc], response, rc);
				bc = bc + rc;
				/* Not all sites are compliant with RFC2616,
				 * as such only test for LF and not CR
				 */
				if (response[rc-1] == '\n')
				{
					break;
				}
			}
			else if (rc == 0)
			{
				fprintf(stderr, "Client closed connection\n");
				close(csd);
				exit(EXIT_FAILURE);
			}
			else
			{
				fprintf(stderr, "Error while reading\n");
				close(csd);
				exit(EXIT_FAILURE);
			}
		}
		buf[bc] = '\0';


		char *curLine = buf;
		/* look for first \n and store it in nextLine*/
		char *nextLine = strchr(curLine, '\n');

		/* Check status of Request, if 200 print body else print 
		   status only*/
		if (strstr(curLine,"HTTP/1.1 200 OK") != NULL)
		{
			char *msg_body, *body;
			msg_body = strstr(curLine, "\r\n\r\n");
			// body = strchr(msg_body, '<');
			printf("%s\n", msg_body);
		}
		else
		{
			if (nextLine)
			{	
				*nextLine = '\0';
			}
			printf("%s\n",curLine);
		}
	}
	else
	{
		printf("Please reconfirm correct input and try again\n");
	}
	if (close(csd) < 0)
	{
		perror("While calling close()");
		exit(EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}