/* Name: inter_P1.c
 * Pourpose: Intepol linear in R^n.
 * Author: dmike
 */

#ifdef MTRACE
#include <mcheck.h>
#endif /* MTRACE */
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include "grid.h"
#include "u0.h"

#define P1(a0,a1,y0,y1) (a0)*(y0)-(a1)*(y1)

struct axes_nod {
	int dim;
	float *nod;
};

static float *interpol_fun(int dim_space,gridType g_nod, gridType g_point,
		float *first,float *step);
static float *find_point_value(int dim_space, int num_vertex,float **point);

static float *set_level_func_value(int num_P1, float *temp_value, 
				   float *point_value);

int 
main(int argc, char *argv[])
{
	gridType g_nod,g_point;
	register int i;
	int dim_nod;
	int dim_space;
	float *first,*last,*step,*new_value;

#ifdef MTRACE
	mtrace();
#endif /* MTRACE */

	// Check the correct usage of the programm	
	if (argc != 2){
		fprintf(stderr,"usage: P1 filename.dat\n");
		exit(EXIT_FAILURE);
	}


	int fd;
	struct stat file_info;
	char *data,*tmp;
	size_t length;

	fd = open (argv[1], O_RDONLY);
	fstat (fd,&file_info);
	length = file_info.st_size;

	data =  (char*) malloc (length);
	read (fd,data,length);
	close(fd);

	tmp = strtok(data,"\n");
	dim_space = atoi(tmp);
	tmp = strtok(NULL, "\n");
	dim_nod = atoi(tmp);
	first = (float*) malloc(dim_space*sizeof(float));
	last = (float*) malloc(dim_space*sizeof(float));
	for (i = 0; (tmp = strtok(NULL, "\n")) != NULL ;i++){
	  first[i]=(float)atof(tmp);
	  last[i] = (float)atof(tmp = strtok(NULL,"\n"));
	}
					
	free((void*)data);
	// Read from the file
	step = malloc(dim_space*sizeof(float));
	for (i = 0; i < dim_space; i++)
		step[i] = (last[i] - first[i])/(dim_nod - 1.00f);

	// Print the values read from file
	fprintf(stdout,"********************************\n");
       	fprintf(stdout,"Values read from %s :\n",argv[1]);
	fprintf(stdout,"Space R^%d\n""Number of nodes %d\n",dim_space,dim_nod);
	fprintf(stdout,"  °n        Axes Range         Spatial Step\n");
	for( i=0; i < dim_space; i++)
		fprintf(stdout," %3d      "      "  [%.2f,%.2f]        "
			"DeltaX=%.2f\n",i+1,first[i],last[i],step[i]);
	fprintf(stdout,"**********************************\n");

	// Create the grid in R^n and the stores the interpol points
	g_nod = create_grid(dim_nod,dim_space,first,step);
	g_point = interpol_points(dim_space,first,last);


	// Interpol the u0 function in the g_points grid
        new_value = interpol_fun(dim_space,g_nod,g_point,first,step);

	// Print the itnerpol value

	for(i = 0; i < g_point[0]->dim; i++)
	  fprintf(stdout," pt %d°, value = %f \n",i+1,new_value[i]);

	clear_grid(g_nod,dim_space);
	clear_grid(g_point,dim_space);
	free((void*)new_value);
	free((void*)step);
	free((void*)first);
	free((void*)last);

	return 0;

}

static float
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

float
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
