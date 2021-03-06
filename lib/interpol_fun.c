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


#define __copy_values(va,lim,tmp,x)		\
  do{for(va = 0; va < lim; va++)		\
      x[va] = tmp[va];				\
  }while(0)						




static inline float
interpol_linear_tree(int dim_space, int num_vertex,const float *g_point, 
		     const float **point, float *point_value)
{
  register int j,k,l,h;

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
	(point[j+mv_][dim_flag-1]-point[j][dim_flag-1]);
      
      // Value get out from interpol linear
      tmp_value[k] = P1(a0,a1,point_value[l],point_value[l+1]);
      l += 2;
      j += add_;
    }
    // Control the interpolation's number and their dimension 
    __copy_values(h,num_P1,tmp_value,point_value);
    num_P1 = num_P1/2;
    ++dim_flag;
    if(num_P1 == 0)continue;
    add_ = num_vertex/num_P1;
    mv_ = add_/2;
    //tmp_value = realloc(tmp_value, num_P1*sizeof(float));
  }while (dim_flag <= dim_space);

  new_value = *point_value;

  free(tmp_value);

  return new_value;

}


#define search_values(dim,dimN,lim,in,nva,oldva)  						\
	do{															 		\
		int __i_ser,__j_ser,__id_ser;									\
		for(__i_ser=0;__i_ser<lim;__i_ser++){							\
			__id_ser=0;													\
			for(__j_ser=dim-1;__j_ser>=0;__j_ser--) 					\
				__id_ser+= in[__i_ser][__j_ser] * (int)pow(dimN,__j_ser);  \
			nva[__i_ser]=oldva[__id_ser];								\
			}																\
		}while(0)

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
interpol_fun(int dim_space,int dim_nod, gridType g_nod,const float *g_point,
	     const float *first, const float *step, float(*f)(int,const float*))
{
  int num_vertex = (int)pow(2,dim_space);
  float new_value;
  float *point_value;
  float **point;
  int **index;

  
  index = find_index_region(g_point,dim_space,dim_nod,first,step);
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
  float **point;
  int **index;
  float *point_value = malloc(num_vertex*sizeof(float));

  if(point_value == NULL){
    fprintf(stderr,"Error in malloc\n");
    abort();
  }

  
  index = find_index_region(g_point,dim_space,dim_nod,first,step);
  point = find_region_grid(dim_space,g_nod,(const int**)index);
  search_values(dim_space,dim_nod,num_vertex,index,point_value,nod_values);
  new_value = interpol_linear_tree(dim_space,num_vertex,g_point,
				   (const float **)point,point_value);    

  free(point_value);
  clear_index((void**)point,dim_space);
  clear_index((void**)index,dim_space);
  
  
  return new_value;
}


