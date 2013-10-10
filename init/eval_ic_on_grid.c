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

  if((index = (int*)malloc(dim_space*sizeof(int))) == NULL){
    fprintf(stdout,"**************************************\n");
    fprintf(stdout," Error in allocate memory             \n");
    abort();
      }

  for(i = 0; i < dim_space; i++)
    index[i] = 0;

  return index;
}


float 
*eval_ic_on_grid(int dim_space, int dim_nod, gridType g_nod, 
		 float (*f)(int,float*))
{
  register int i,j;

  int grid_size = (int) pow(dim_nod,dim_space);
  int *index;
  float *nod_values,*point;
  
  index = init_index(dim_space);
  nod_values = (float*)malloc(grid_size*sizeof(float));

  if(nod_values == NULL){
    fprintf(stdout,"Error in allocate memory\n");
    abort();
  }

  for(i = 0; i < grid_size; i++){
    point = find_point(dim_space,index,g_nod);
    nod_values[i] = f(dim_space,point);
    free((void*)(point));
    for(j = 1; j < dim_space; j++)
      if(index[dim_space-j] == dim_nod-1){
	index[dim_space-j] = 0;
	if(index[dim_space-j-1] != dim_nod-1){
	  index[dim_space-j-1]++;
	  break;}}
      else{
	index[dim_space-j]++;
	break;}
  }

  free((void*)index);

  return nod_values;
}
      
      
