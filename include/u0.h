#ifndef U0_H
#define U0_H

#include <math.h>

inline float 
u_0(int n, const float *point, float radius)
{
  const float *p;
  float u = 0.00f;
  float r_two = radius*radius;
  
  for (p = point; p <point + n; p++)
    u += (*p)*(*p); 
  if (u <= r_two)
    return powf((r_two-u)/(r_two),4);
  else 
    return 0.0f;
  
}


#endif
