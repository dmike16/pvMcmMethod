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
  float max_uap = -1.00f;
  float tmp;

  for(i = 0; i < dim_vec; i++){
    tmp = fabs(u_exact[i]-u_appx[i]);
    if( tmp > norm_inf)
      norm_inf = tmp;
    if(fabs(u_appx[i]) > max_uap)
      max_uap = fabs(u_appx[i]);
  }
  
  
  fprintf(stdout,"The Nom Inf: ||u(x,t)-v(x,t)|| = %.4f\n",norm_inf);
  fprintf(stdout,"The Max of u apprx:  %.4f\n",max_uap);

}
		  
