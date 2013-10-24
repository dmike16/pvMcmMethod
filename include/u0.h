#ifndef U0_H
#define U0_H

#include <math.h>

inline float 
u_0(int n, const float *point)
{
  const float *p;
  float u = 0.00f;
  
  
  for (p = point; p <point + n; p++)
    u += (*p)*(*p); 
  if (sqrt(u) <= 1)
    return 1.0f-u;
  
  else 
    return 0.0f;
  
}


#endif
