/*
 * This servver will accept a UDP message consisting of a text string sent to it
 * by a client process. The server will take the string and send it back to the  
 * client in reverse order.
 * Compile with: cc 16965270-set-1-question-1-server.c −o echo_serv
 * Usage: ./echo_serv server_port max)iterations
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

/* Make comment here */
int reverse_string(char *inputStr, char *outputStr)
{
	int i, len;

	len = strlen(inputStr);
	for (i = 0; i < len; i++)
	{
		outputStr[i] = inputStr[len-1-i];
	}
	outputStr[len] = '\0';
	return len;
}

main(int argc, char *argv[])
{
	int ssd;			/* Server socket descriptor	*/
	struct sockaddr_in server;	/* Server address structure	*/	
	struct sockaddr_in client;	/* Client address structure	*/
	int client_len;			/* Size of client structure	*/
	short echo_port;		/* Server's port number		*/
	int max_iterations;		/* Max iterations to perform	*/
	int out_cnt, in_cnt;		/* Byte count for TX and RX	*/
	int recv_cnt, i;		/* Generic counters		*/
	char client_string[BUF_LEN];	/* Buffer to hold RX string 	*/
	char server_reverse_string[BUF_LEN];
					/* Buffer to hold TX string 	*/
	int ret_code;			/* Generic return  		*/

	/* Check if number of required arguments are satisifed	*/
	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s Port max_iterations\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Decode command line arguments supplied	*/
	echo_port = atoi(argv[1]);
	max_iterations = atoi(argv[2]);

	/* 
	 * Create the socket
	 * PF_INET: The Internet (TCP/IP) family.
	 * SOCK_DGRAM : The type of service required - datagram
	 * 17: UDP protocol
	 */
	ssd = socket(PF_INET, SOCK_DGRAM, 17);
	/* If error occurs, report problem and terminate	*/
	if (ssd < 0)
	{
		perror("While creating socket()");
		exit(EXIT_FAILURE);
	}

	/* 
	 * Setting up the server in order to bind time to specified socket.
	 * Use Internet address family - AF_INET
	 * Use INADDR_ANY, allows servers to receive message sent to any of its
	 * interfaces.
	 * Convert the port number from host to network order, there sometimes is 
	 * a difference.
	 */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(echo_port);

	/* Bind the server sockaddr_in structure to the socket	*/
	ret_code = bind(ssd, (struct sockaddr *)&server, sizeof(server));
	if (ret_code < 0)
	{
		perror("While calling bind()");
		exit(EXIT_FAILURE);
	}

	/* Limit server to number of request (max_iterations)	*/
	for (i = 0; i < max_iterations; i++)
	{
		fprintf(stderr, "Iteration %d of %d. Waiting for client..\n",
				i+1, max_iterations);
		client_len = sizeof(client);
		/*
		 * recvfrom() system call will block until message is received
		 * from a client.
		 */
		in_cnt = recvfrom(ssd, client_string, BUF_LEN, 0,
					(struct sockaddr *)&client,
					(socklen_t *)&client_len);
		if (in_cnt < 0)
		{
			perror("While calling recvfrom()");
			exit(EXIT_FAILURE);
		}
		fprintf(stderr, "Message received is %d bytes long\n", in_cnt);
		fprintf(stderr, "Message received is \"%s\"\n", client_string);
		/* Reverse the string 	*/
		recv_cnt = reverse_string(client_string, server_reverse_string);

		fprintf(stderr, "Reversed string is %d bytes long\n", recv_cnt);
		fprintf(stderr, "Reversed string is \"%s\"\n", 
				server_reverse_string);

		out_cnt = sendto(ssd, server_reverse_string, recv_cnt+1, 0, 
					(struct sockaddr *)&client, 
					sizeof(client));
		if (out_cnt < 0)
		{
			perror("While calling sendto()");
			exit(EXIT_FAILURE);
		}
		fprintf(stderr, "Client request now serviced, reply sent.\n");
	}
	close(ssd);
	fprintf(stderr, "Server has shut down.\n");
	return 0;
}
