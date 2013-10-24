/* Name: interpol_fun.c
 * Pourpose: Intepol linear in R^n.
 * Author: dmike
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "grid.h"
#include "interpol_fun.h"

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


static float
interpol_linear_tree(int dim_space, int num_vertex,const float *g_point, 
		     const float **point, float *point_value)
{
  register int j,k,l;

  int mv_,add_,dim_flag,num_P1;
  float new_value;
  float a0,a1;

  // Sequence of linear interpolation
  
  num_P1 = num_vertex/2;  
  dim_flag = 1;
  add_ = 2;
  mv_ = 1;

  float *tmp_value = malloc(num_P1*sizeof(float));

  do {
    j = 0;
    l = 0;
    for (k = 0; k < num_P1; k++){
      // Coefficintes for interpol linear
      a0 = (g_point[dim_flag-1]-point[j+mv_][dim_flag-1])/
	(point[j][dim_flag-1]-point[j+mv_][dim_flag-1]);
      
      a1 = (g_point[dim_flag-1]-point[j][dim_flag-1])/
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

  new_value = *point_value;

  free((void*)tmp_value);

  return new_value;

}

static float 
*find_point_value_discrete(int dim_space, int dim_nod, int num_vertex, 
			   const int **index,const float *nod_values)
{
  register int i,j;
  int id;
  float *value = malloc(num_vertex*sizeof(float));

  if(value == NULL){
    fprintf(stdout,"Error in malloc\n");
    abort();
  }

  for (i = 0; i < num_vertex; i++){
    id = 0;
    for (j = dim_space-1; j >= 0; j--)
      id += (int) pow(dim_nod,j)*index[i][j];
    value[i]=nod_values[id];
  } 
    

  return value;
}

static float 
*find_point_value(int dim_space, int num_vertex, const float **point,
		  float(*f)(int,const float*))
{
  register int i;
  float *value = malloc(num_vertex*sizeof(float));

  if(value == NULL){
    fprintf(stdout,"Error in malloc\n");
    abort();
  }

  for (i = 0; i < num_vertex; i++)
    value[i] = f(dim_space, point[i]);

  return value;
}

float
interpol_fun(int dim_space,gridType g_nod,const float *g_point,
	     const float *first, const float *step, float(*f)(int,const float*))
{
  int num_vertex = (int)pow(2,dim_space);
  float new_value;
  float *point_value;
  float **point;
  int **index;

  
  index = find_index_region(g_point,dim_space,first,step);
  point = find_region_grid(dim_space,g_nod,(const int**)index);
  point_value = find_point_value(dim_space,num_vertex,(const float**)point,f);   
  
  new_value = interpol_linear_tree(dim_space,num_vertex,g_point,
				   (const float**)point,point_value);    
  
  free((void*)point_value);
  clear_index((void**)point,dim_space);
  clear_index((void**)index,dim_space);
  
  
  return new_value;
}


float
interpol_fun_discrete(int dim_space,int dim_nod,gridType g_nod, 
		      const float *g_point,const float *first,const float *step,
		      const float *nod_values)
{

  int num_vertex = (int)powf(2,dim_space);
  float new_value;
  float **point,*point_value;
  int **index;

  
  index = find_index_region(g_point,dim_space,first,step);
  point = find_region_grid(dim_space,g_nod,(const int**)index);
  point_value = find_point_value_discrete(dim_space,dim_nod,num_vertex,
					  (const int**)index,nod_values);

  new_value = interpol_linear_tree(dim_space,num_vertex,g_point,
				   (const float **)point,point_value);    

  free((void*)point_value);
  clear_index((void**)point,dim_space);
  clear_index((void**)index,dim_space);
  
  
  return new_value;
}


