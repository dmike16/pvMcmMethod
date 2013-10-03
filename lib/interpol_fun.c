/* Name: interpol_fun.c
 * Pourpose: Intepol linear in R^n.
 * Author: dmike
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "grid.h"
#include "u0.h"
#include "interpol_fun.h"


 float
*interpol_fun(int dim_space,gridType g_nod,gridType g_point,
		float *first,float *step)
{
  register int i,j,k,l;

  int mv_,add_,dim_flag,num_P1,num_vertex = (int)powf(2,dim_space);
  float *new_value = malloc((g_point[0]->dim)*sizeof(float));
  float a0,a1;
  
  if (new_value == NULL){
    fprintf(stdout,"**Error: Can't allocate memory\n");
    exit(EXIT_FAILURE);
  }
  
  for (i = 0; i < g_point[0]->dim; i++){
    dim_flag = 1;
    num_P1 = num_vertex/2;
    add_ = 2;
    mv_ = 1;

    float *tmp_value = malloc(num_P1*sizeof(float));
    float *point_value;
    float **point;
    int **index;


    index = find_index_region(g_point,dim_space,first,step,i);
    point = find_region_grid(dim_space,g_nod,index);
    point_value = find_point_value(dim_space,num_vertex,point);
    
    // Sequence of linear interpolation
    
    do {
      j = 0;
      l = 0;
      for (k = 0; k < num_P1; k++){
	// Coefficintes for interpol linear
	a0 = (g_point[dim_flag-1]->nod[i]-point[j+mv_][dim_flag-1])/
	  (point[j][dim_flag-1]-point[j+mv_][dim_flag-1]);

	a1 = (g_point[dim_flag-1]->nod[i]-point[j][dim_flag-1])/
	  (point[j][dim_flag-1]-point[j+mv_][dim_flag-1]);

	// Value get out from interpol linear
	tmp_value[k] = P1(a0,a1,point_value[l],point_value[l+1]);
	l += 2;
	j += add_;
      }
	// Control the interpolation's number and their dimension 
	point_value = set_level_func_value(num_P1,tmp_value,point_value);
	num_P1 = num_P1/2;
	++dim_flag;
	if(num_P1 == 0)continue;
	add_ = num_vertex/num_P1;
	mv_ = add_/2;
	tmp_value = (float*)realloc((void*)tmp_value, num_P1*sizeof(float));
    }while (dim_flag <= dim_space);
    
    new_value[i] = *point_value;
    
    free((void*)point_value);
    free((void*)tmp_value);
    clear_index((void**)point,dim_space);
    clear_index((void**)index,dim_space);
  }
  
  return new_value;
}

float 
*find_point_value(int dim_space, int num_vertex, float **point)
{
  register int i;
  float *value = malloc(num_vertex*sizeof(float));

  if(value == NULL){
    fprintf(stdout,"Error in malloc\n");
    abort();
  }

  for (i = 0; i < num_vertex; i++)
    value[i] = u_0(dim_space, point[i]);

  return value;
}

static float
*set_level_func_value(int num_P1, float *tmp_value, float *point_value)
{
  register int i;

  if((point_value = (float*)realloc((void*)point_value,num_P1*sizeof(float))) == NULL)
    {
      fprintf(stdout,"Error in realloc\n");
      abort();
    }

  for(i = 0; i < num_P1; i++)
    point_value[i] = tmp_value[i];

  return point_value;
}
