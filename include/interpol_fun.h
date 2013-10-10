#ifndef INTERPOL_H
#define INTERPOL_H

#define P1(a0,a1,y0,y1) (a0)*(y0)-(a1)*(y1)

float interpol_fun(int dim_space,gridType g_nod, float *g_point,
		    float *first,float *step,float(*f)(int,float*));

float interpol_fun_discrete(int dim_space,int dim_nod,gridType g_nod,
			    float *g_point,float *first,float *step, 
			    float *nod_values);

#endif
