#ifndef USPHERE_H
#define USPHERE_H

#include <math.h>

extern float timeto;

inline float 
u_sphere(int n, const float *point, float radius)
{
  const float *p;
  float u = 0.00f;
  float r_two = radius*radius;
 

  for (p = point; p <point + n; p++)
    u += (*p)*(*p); 
  if (u <= r_two-2.00f*timeto)
    return powf((r_two-2.00f*timeto-u)/r_two,4);
  else 
    return 0.0f;
  
}


#endif
