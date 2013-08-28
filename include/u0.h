#ifndef U0_H
#define U0_H

#include <math.h>

inline float 
u_0(int n,float *point)
{
  register int i;
  float u = 0.00f;
  
  
  for (i = 0; i < n; i++)
    u += point[i]*point[i]; 
  if (sqrt(u) <= 1)
    return 1.0f-u;
  
  else 
    return 0.0f;
  
}


#endif
