/* Name: eval_method_errno.c
 * Pourpose: eval the error of the apprx solution within the exact sol
 * Author: dmike
 */

#include <stdio.h>
#include <math.h>
#include "eval_method_errno.h"

void 
eval_method_errno(const float *u_appx, const float *u_exact,int dim_vec)
{
  register int i;

  float norm_inf = -1.00f;

  for(i = 0; i < dim_vec; i++)
    if(fabs(u_exact[i]-u_appx[i]) > norm_inf)
      norm_inf = fabs(u_exact[i]-u_appx[i]);

  fprintf(stdout,"The INF NORM is: %f\n",norm_inf);

}
		  
