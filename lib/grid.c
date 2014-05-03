/* Name: grid.c
 * Pourpouse: Define the structure for interface grid.h
 * Author: dmike
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "grid.h"

#define exit_failure 1
#define exit_success 0

struct axes_nod {
	int dim;
	float *nod;
};


#define check(p,msg)					\
	do{									\
		if(p==NULL){					\
			fprintf(stderr,"%s\n",msg); \
			exit(exit_failure);			\
		}								\
	}while(0)

#define find_pos(va,dim,in,pt,fpt,step) 					\
	do{														\
		for(va = 0; va < dim; va++)							\
			in[va]=(int) ((pt[va] - fpt[va])/step[va]);		\
	}while(0)



static gridType 
init_grid(int dim_space)
{
	gridType g = malloc(dim_space*sizeof(struct axes_nod*));

	check(g,"**Error during allocaton of memory \n");

	return g;
}

gridType
create_grid(int dim_nod, int dim_space,const float *first,
		const float *step)
{
	register int i,j;
	gridType g = init_grid(dim_space);


	for(i = 0; i < dim_space; i++){
	  g[i] = malloc(sizeof(struct axes_nod));
	  check(g[i],"Error in create grid.");
	  g[i]->dim = dim_nod;
	  g[i]->nod = malloc(dim_nod*sizeof(float));
	  check(g[i]->nod,"Error in create nodes partition");
	  for( j = 0; j < dim_nod; j++)
	    g[i]->nod[j] = first[i] + step[i]*j;
	}

	return g;
}


float
*find_point(int dim_space,const int *index, gridType g, float *point)
{
	register int i;
	float *vpoint = point;

	for (i = 0; i < dim_space; i++)
		vpoint[i] = g[i]->nod[index[i]];

	return vpoint;
}

float
**find_region_grid(int dim_space,gridType g,const int **index)
{
	register int i;
	int num_vertex = (int) pow(2,dim_space);
	float **g_reg;

	
	if((g_reg = malloc(num_vertex*sizeof(float*))) == NULL)
	   fprintf(stdout,"**ERROR IN ALLOCATE MEMORY\n");
	

	for (i = 0; i < num_vertex; i++){
	  g_reg[i] = malloc(dim_space*sizeof(float));
	  check(g_reg[i],"**Error during allocaton of memory \n");
	  find_point(dim_space,index[i],g,g_reg[i]);
	}
	
	return g_reg;
}

int
**find_index_region(const float *g, int dim_space, const float *first, 
		    const float *step)
{
  register  int i,j; //mv_ = 0,mv_up = 1,mv_tmp = 0,i_tmp = 0,i_up,flag;
  int num_vertex = (int) powf(2,dim_space);
  int **index = malloc(num_vertex*sizeof(int*));
  
  index[0] = malloc(dim_space*sizeof(int));
  check(index[0],"**Error during allocation of memory \n");
  find_pos(i,dim_space,index[0],g,first,step);
  //flag = 1,i_up = 0;
  
  for (i = 1; i < num_vertex; i++){
    index[i] = malloc(dim_space*sizeof(int));
    check(index[i],"**Error during allocation of memory \n");
    for(j=0; j < dim_space; j++)
    	if(i & 1 << j)
    		index[i][j]=index[0][j]+1;
    	else
    		index[i][j]=index[0][j];
    /*if(mv_tmp != 0){
      i_tmp = i_up;
      if (flag == 4){
	mv_up ++;
	mv_ = mv_up;
	flag = 0;
	i_up = i;
      }
      else 
	mv_ = 1;
      
    }
    else {
      mv_ = 0;
      i_tmp = i-1;
    }
    if( mv_up == dim_space -1)mv_up = 1;		       	       
    for (j = 0; j < dim_space; j++)
      if (j == mv_)
	index[i][j] = index[i_tmp][mv_] + 1;
      else
	index[i][j] = index[i_tmp][j];
    flag++;
    if (mv_tmp == 0)
      mv_tmp++;
    else
      mv_tmp--;*/
  }
  
  return index;
}

int
output_axes_nod(gridType g, int dim_space, char *filename)
{
  register int i,j;
  int char_numb = 0;
  FILE *xog;

  xog = fopen(filename,"w");
  
  for(i = 0; i < g[0]->dim; i++){
    for(j = 0; j < dim_space; j++)
      char_numb = fprintf(xog,"%.6f    ",g[j]->nod[i]);
    char_numb = fprintf(xog,"\n");
  }
  fclose(xog);					
  return char_numb;
}


void 
clear_grid (gridType g,int dim_space)
{
  register int i;
  
  for (i = 0; i < dim_space; i++){
    free((void*)g[i]->nod);
    free((void*)g[i]);
  }
  
  free((void*)g);
}

void 
clear_index(void **p,int dim_space)
{
  register int i;
  int num_vertex = (int) powf(2,dim_space);
  
  for ( i = 0; i < num_vertex; i++)
    free((void*)p[i]);
  free((void*)p);
}

