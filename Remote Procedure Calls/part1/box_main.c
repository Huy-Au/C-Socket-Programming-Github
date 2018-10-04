/* box_main.c
   Performs calcuation about boxes using the remote procedure calls
   This is the client code
   Compile it with the rpcgen-ed code

   cc box_main.c box_clnt.c box_xdr.c -o box_client
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"box.h"

/* Print out the details of the box
   Input: Takes five floats
   Output: Void, prints out details of the box
*/
void print_box(float l, float w, float h, float sa, float v)
{
	printf("A box of dimensions of %f x %f x % f\n", l, w, h);
	printf("Has a surface area of %f\n", sa);
	printf("and a volume of %f\n", v);
	return;
}

int main(int ac, char** av)
{
	CLIENT *c_handle;
	dimensions dims;
	box_results *res;

	/* Check correct usage of command line arguments */
	if (ac != 5)
	{
		fprintf(stderr, "Usage: %s remote_host length width height\n", av[0]);
		exit(EXIT_FAILURE);
	}

	/* Grab values from the command line argument and insert into struct */
	dims.length = atof(av[2]);
	dims.width = atof(av[3]);
	dims.height = atof(av[4]);
	printf("Sending %f, %f, %f\n", dims.length, dims.width, dims.height);
	/* Create a connection handle to make rpc's */
	c_handle = clnt_create(av[1], RPC_BOX, BOXVERSION1, "udp");
	if (c_handle == NULL)
	{
		clnt_pcreateerror(av[1]);
		exit(EXIT_FAILURE);
	}

	/* Make the rpc */
	res = box_calc_1(&dims, c_handle);

	if (res == NULL)
	{
		clnt_perror(c_handle, av[1]);
		exit(EXIT_FAILURE);
	}

	/* Print out the results */
	print_box(dims.length, dims.width, dims.height, res->surface, 
		res->volume);

	/* Free up the rpc handle */
	clnt_destroy(c_handle);
	return 0;
}