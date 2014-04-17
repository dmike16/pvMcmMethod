/* Name: eval_ic_on_grid.c
 * Pourpose: eval the initial condition on the grid nods
 * Author: dmike
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "grid.h"
#include "eval_ic_on_grid.h"

static int
*init_index(int dim_space)
{
  register int i;

  int *index;

  if((index = malloc(dim_space*sizeof(int))) == NULL){
    fprintf(stdout,"**************************************\n");
    fprintf(stdout," Error in allocate memory             \n");
    abort();
      }

  for(i = 0; i < dim_space; i++)
    index[i] = 0;

  return index;
}


float 
*eval_ic_on_grid(int grid_size,int dim_space, int dim_nod, gridType g_nod, 
		 float (*f)(int,const float*,const float*),const float *radius)
{
  register int i,j;

  int *index;
  float *nod_values,*point;
  
  index = init_index(dim_space);
  nod_values = malloc(grid_size*sizeof(float));

  if(nod_values == NULL){
    fprintf(stdout,"Error in allocate memory\n");
    abort();
  }

  point = malloc(dim_space*sizeof(float));

  if(point == NULL){
    fprintf(stdout,"Error in allocate memory\n");
    abort();
  }
  
  for(i = 0; i < grid_size; i++){
    find_point(dim_space,index,g_nod,point);
    nod_values[i] = f(dim_space,point,radius);
    for(j = 0; j < dim_space; j++)
      if(index[j] == dim_nod-1)
	index[j] = 0;
      else{
	++index[j];
	break;}
  }

  free(point);
  free(index);

  return nod_values;
}
      
      
