#ifndef INTERPOL_H
#define INTERPOL_H

#define P1(a0,a1,y0,y1) (a0)*(y0)-(a1)*(y1)

struct axes_nod {
	int dim;
	float *nod;
};

float *interpol_fun(int dim_space,gridType g_nod, gridType g_point,
	float *first,float *step);
float *find_point_value(int dim_space, int num_vertex,float **point);

static float *set_level_func_value(int num_P1, float *temp_value, 
				   float *point_value);

#endif
