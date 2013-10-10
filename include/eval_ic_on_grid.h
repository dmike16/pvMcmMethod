#ifndef EVAL_IC_ON_GRID_H
#define EVAL_IC_ON_GRID_H

float *eval_ic_on_grid(int dim_space, int dim_nod, 
		       gridType g_nod,float (*f)(int,float*));
#endif
