/*
 * u0_Torus.h
 *
 *  Created on: Mar 26, 2014
 *      Author: dmike
 */

#ifndef U0_TORUS_H_
#define U0_TORUS_H_

#include <math.h>


inline float
u0_torus(int n, const float *point,const float *r)
{
  register int i;
  float u = 0.00f, u_z = 0.00f;
  float r_two = r[0]*r[0];

  for (i = 0; i <  n; i++){
	  if(i !=  n -1)
		  u += (point[i])*(point[i]);
	  else
		  u_z += (point[i])*(point[i]);
  }

    return r_two - powf((r[1]-sqrt(u)),2) - u_z;

}



#endif /* U0_TORUS_H_ */
