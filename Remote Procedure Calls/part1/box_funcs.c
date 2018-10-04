/* box_funcs.c
   Server part for our rpc demo

   compile with open rpcgen-ed files
   cc box_funcs.c box_svc.c box_xdr.c -o box_svc
*/

#include "box.h"

/* Calculates the surface area of a 3 dimension object
   Input: three floats representing the dimension of the object
   Output: single float representing the surface area
*/
float surface(float l, float w, float h)
{
	return 2*(l*w + l*h + w*h);
}

/* Calculates the volume of a 3 dimensional object
   Input: three floats representing the dimension of the object
   Output: single float representing the volume of the object
*/
float volume(float l, float w, float h)
{
	return l*w*h;
}

/* The remote procedure */
box_results* box_calc_1_svc(dimensions *dims, struct svc_req *rqstp)
{
	/* Structure requires to be static as we return a pointer to it */
	static box_results res;
	res.surface = surface(dims->length, dims->width, dims->height);
	res.volume = volume(dims->length, dims->width, dims->height);
	return &res;

}