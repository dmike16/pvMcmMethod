/*
 * mcmschema.h
 *
 *  Created on: May 7, 2014
 *      Author: dmike
 */

#ifndef MCMSCHEMA_H_
#define MCMSCHEMA_H_

extern void mcmschema(int dim_space,int grid_size, int dim_nod,
			  float *u_n_plus_one, const float *u_n,
			  const float *step, float delta_t, gridType g_nod,
			  const float *first, const float *last);


#endif /* MCMSCHEMA_H_ */
