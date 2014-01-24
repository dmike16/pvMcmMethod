#ifndef PVSCHEMA_CORE_H
#define PVSCHEMA_CORE_H

extern float vol_preserv;
extern void pvschema_core(int dim_space,int grid_size, int dim_nod,
			  float *u_n_plus_one, const float *u_n,
			  const float *step, float delta_t, gridType g_nod,
			  const float *first, const float *last);

#endif /*END pvschema_core.h*/
