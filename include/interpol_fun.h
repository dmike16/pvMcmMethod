#ifndef INTERPOL_H
#define INTERPOL_H

#define P1(a0,a1,y0,y1) (a0)*(y0)-(a1)*(y1)

extern float interpol_fun(int dim_space,gridType g_nod, const float *g_point,
			  const float *first,
			  const float *step, float(*f)(int,const float*));

extern float interpol_fun_discrete(int dim_space,int dim_nod,gridType g_nod,
				   const float *g_point, const float *first,
				   const float *step, const float *nod_values);

#endif
