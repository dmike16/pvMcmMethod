#ifndef EVAL_IC_ON_GRID_H
#define EVAL_IC_ON_GRID_H

extern float *eval_ic_on_grid(int grid_size,int dim_space, int dim_nod, 
			      gridType g_nod,float (*f)(int,const float*,float),
			      float radius);
#endif
