/* Name: out_boundary.c
 * Pourpose: check if we are on the boundary or not. False on the boundary,
 * true out of boundary.
 * Author: dmike
 */

#include "out_boundary.h"

#define true       1
#define false      0

bool
out_boundary(int dim_space, int dim_nod,int *index)
{
  register int i,nb_in = 0;
  bool in = false;

  for(i = 0;i < dim_space; i++)
    if(index[i] != 0 && index[i] != dim_nod-1)
      ++nb_in;
 
  if(nb_in == dim_space)
    in = true;

  return in;

}
