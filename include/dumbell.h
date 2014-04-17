/*
 * dumbell.h
 *
 *  Created on: Apr 16, 2014
 *      Author: dmike
 */

#ifndef DUMBELL_H_
#define DUMBELL_H_

#include <math.h>

#define chi(a,b,x) ((x>=a && x<=b) ? 1.00f : 0.00f)

inline float
u0_dumbell(int n, const float *point,const  float *radius)
{
  float u = 0.00f;
  float r = radius[0]*radius[0];
  float R = radius[1]*radius[1];

  float alt = 1-sqrt(R-r);
  float u1,u2,u3;


  u1 = (chi(-alt,alt,point[0]))*(-point[1]*point[1]-point[2]*point[2]+r);
  u2 = R-((point[0]-1)*(point[0]-1) + point[1]*point[1] + point[2]*point[2]);
  u3 = R-((point[0]+1)*(point[0]+1) + point[1]*point[1] + point[2]*point[2]);

  if(u1 > u) u = u1;
  if(u2 > u) u = u2;
  if(u3 > u) u = u3;

  return u;

}

#endif /* DUMBELL_H_ */
