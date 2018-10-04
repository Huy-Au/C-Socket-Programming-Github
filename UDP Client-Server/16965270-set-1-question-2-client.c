/*
 * This client will send a string to a server process on the same machine or 
 * a different host.
 * The server will echo the string back in reverse order.
 * Will use connect(), write() and read()
 * Compile with: cc 16965270-set-1-question-2-client.c −o connectit
 * Usage: ./connectit server_port string
 */
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>

#define BUF_LEN 48

main(int argc, char *argv[])
{
	int csd;			/* client socket descriptor	*/
	int cnt_status;			/* Connection status 		*/
	struct sockaddr_in server;	/* server address socket	*/
	struct hostent *server_host;	/* pointer to server host details
					 * structure of above structure
					 */
	int server_len;			/* size of above structure	*/
	int string_size;		/* String size including null	*/
	short server_port;		/* servers port numner		*/
	int out_cnt, in_cnt;		/* Byte count for RX and TX 	*/
	char client_send_string[BUF_LEN];
					/* Buffer to hold send string 	*/
	char server_reversed_string[BUF_LEN];
					/* Buffer for received string 	*/

	/* Verify for correct number of command line arguments	*/
	if (argc != 4)
	{
		fprintf(stderr, "Usage %s Server Port send_string\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Decode command line arguments	*/
	server_host = gethostbyname(argv[1]);
	/* Report and terminate program if error detected	*/
	if (server_host == NULL)
	{
		herror("While calling gethostbyname()");
		exit(EXIT_FAILURE);
	}
	server_port = atoi(argv[2]);
	strcpy(client_send_string, argv[3]);

	/* Create the socket	*/
	csd = socket(PF_INET, SOCK_DGRAM, 0);
	if (csd < 0)
	{
		perror("While calling socket()");
		exit(EXIT_FAILURE);
	}

	/* Set up server address details 	*/
	server.sin_family = AF_INET;
	memcpy(&server.sin_addr.s_addr, server_host->h_addr_list[0],
		server_host->h_length);
	server.sin_port = htons(server_port);

	/* Create connection 	*/
	cnt_status = connect(csd, (struct sockaddr *)&server, sizeof(server));

	/* Report any errors and terminate program if connection unsuccessful */
	if (cnt_status < 0)
	{
		perror("While calling connect()");
		exit(EXIT_FAILURE);
	}

	/* Set the length so trailing null is also sent 	*/
	string_size = strlen(client_send_string) + 1;

	/* Write message to server 	*/
	out_cnt = write(csd, client_send_string, string_size);

	/* Report any errors and terminate program 	*/
	if (out_cnt < 0)
	{
		perror("While calling write()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "You have sent \"%s\"\n", client_send_string);
	fprintf(stderr, 
		"Have reached recvfrom(), should now block until message receipt\n");

	/* Read response from server 	*/
	server_len = sizeof(server);
	in_cnt = read(csd, server_reversed_string, BUF_LEN);

	/* Report any errors and terminate program 	*/
	if (in_cnt < 0)
	{
		perror("While calling read()");
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "The server has responded with: \"%s\"\n", 
				server_reversed_string);
	/* Close the socket now 	*/
	close(csd);


}