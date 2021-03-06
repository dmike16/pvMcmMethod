#ifndef GRID_H
#define GRID_H
#define DEBUG


typedef struct axes_nod **gridType;


extern gridType create_grid( int dim_nod, int dim_space, 
		      const float *first,const float *step);

//gridType interpol_points(int dim_space,float *first,float *last);

extern int *find_position(int dim_space, const float *g, const float *first,
			  const float *step);

extern float *find_point(int dim_space, const int *index, gridType g,
			 float *point);

extern int **find_index_region(const float *g, int dim_space, int dim_nod,
			       const float *first, const float *step);

extern float **find_region_grid(int dim_space, gridType g,const int **index);

extern int output_axes_nod(gridType g, int dim_space,char *filename);

extern void clear_index(void **p, int dim_space);

extern void clear_grid(gridType g,int dim_space);

#define index_cycle(var,dim,dim_n,x)	\
	do{									\
		for(var = 0; var < dim; var++){	\
			if(x[var] == dim_n-1)		\
				x[var] = 0;				\
			else{					    \
				++x[var];				\
				break;}}				\
		}while(0)

#endif
