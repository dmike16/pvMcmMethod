/* Name: u_sphere.c
 * Pourpose: The exact soution in the case of the sphere.
 * Author: dmike
 */

#include "u_sphere.h"

extern float u_sphere(int n, const float *point, float radius);

float 
extract_radius_sphere(float radius,float exp_smooth,float level_set)
{
  return sqrt(radius*radius-powf(level_set,1.00f/exp_smooth)*radius*radius);
}
