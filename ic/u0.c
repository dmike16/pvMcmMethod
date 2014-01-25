/* Name: u0.c
 * Pourpose: Define a inizial func to inperlo.
 * Author: dmike
 */

#include "u0.h"

extern float u_0(int n, const float *point, float radius);

float 
extract_radius_sphere(float radius,float exp_smooth,float level_set)
{
  return sqrt(radius*radius-powf(level_set,1.00f/exp_smooth)*radius*radius);
}
