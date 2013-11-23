/* Name: out_of_grid_check.c
 * Pourpose: Check if the interpol point is out of the grid.
 * Author: dmike
 */

#include "out_of_grid_check.h"

void
out_of_grid_check(int dim,const float *first,const float *last, float *pt)
{
  register int i;

  for(i = 0; i < dim; i++)
    if(pt[i] < first[i])
      pt[i] = first[i];
    else if(pt[i] > last[i])
      pt[i] = last[i];

}
