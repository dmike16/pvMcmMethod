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
  register int i,pos = 0;

  float norm_inf = -1.00f;
  float tmp;

  for(i = 0; i < dim_vec; i++){
    tmp = fabs(u_exact[i]-u_appx[i]);
    if( tmp > norm_inf){
      pos = i;
      norm_inf = fabs(u_exact[i]-u_appx[i]);}}

  fprintf(stdout,"The INF_NORM|%.2f-%.2f| = %.2f\n",u_exact[pos],
	  u_appx[pos],norm_inf);

}
		  
