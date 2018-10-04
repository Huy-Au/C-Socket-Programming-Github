#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define BUF_LEN 10000

int main(int argc, char *argv[])
{
	int csd;	/* Client socket descriptor */
	struct sockaddr_in server;
	struct hostent *server_host;
	int string_size;
	short server_port;
	int send_request, inc_response;
	char response[BUF_LEN];
	char header[BUF_LEN];
	int writeStatus;

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
		printf("Request sent is %d size should be %d\n", writeStatus, 
		       (int)strlen(header));
	} else
	{
		printf("Request sent\n");
	}

	/* Read response from server */
	/* HEAD method can not guarantee content-length key, host dependent 
	 * Content length not checked in this section
	  */
	if ((inc_response = read(csd, response, BUF_LEN)) < 0)
	{
		perror("While receiving response from server");
		exit(EXIT_FAILURE);
	}
	/* Null terminate end of response as buffer set as large value */
	response[inc_response-1] = '\0';

	char *curLine = response;
	while(curLine)
	{
		/* Replace \n with \0 to prevent entire response being printed
		 * while searching for key-value pairs
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

	if (close(csd) < 0)
	{
		perror("While calling close()");
		exit(EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}