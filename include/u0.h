#ifndef U0_H
#define U0_H

#include <math.h>

extern float extract_radius_sphere(const float *radius, float exp_smooth,
				   float level_set);

inline float 
u_0(int n, const float *point,const  float *radius)
{
  const float *p;
  float u = 0.00f;
  float r_two = (*radius) * (*radius);
  
  for (p = point; p <point + n; p++)
    u += (*p)*(*p); 
  if (u <= r_two)
    //return powf((r_two-u)/(r_two),4);
	  return r_two - u;
  else 
    return 0.0f;
  
}


#endif
