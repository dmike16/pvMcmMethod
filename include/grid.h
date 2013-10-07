#ifndef GRID_H
#define GRID_H
#define DEBUG


typedef struct axes_nod **gridType;

gridType init_grid(int dim_space);

gridType create_grid(const int dim_nod,const int dim_space, 
		     const float *first, const float *step);

gridType interpol_points(int dim_space,float *first,float *last);

int *find_position(int dim_space,int point_num, gridType g, 
		   float *first, float *step);

float *find_point(int dim_space,int *index, gridType g);

int **find_index_region(gridType g, int dim_space, float *first, float *step,
		int num_point);

float **find_region_grid(int dim_space, gridType g, int **index);

float *eval_vertex_region(int dim_space, int num_p, gridType g_nod, 
		gridType g_plus,float *first, float *step, 
		float (*f)(int,float*));

void clear_index(void **p, int dim_space);

void clear_grid(gridType g,int dim_space);

#endif
