/* Name: eval_direction_vector.c
 * Pourpose: approximate the vector v_1 and v_2 , autovector of the projection 
 * matrix P(p)=I-(pp^t)/(|p|^2).
 * Author: dmike
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include <string.h>
#include "grid.h"
#include "interpol_fun.h"
#include "mcmschema.h"
#include "signal_dim.h"
#include "out_boundary.h"
#include "out_of_grid_check.h"


#define DIM_SPACE  3
#define NUM_VEC    2
#define C          0.1f
#define _step(x)   sqrt(2.00f*(x))
#define _z_dim(x)  (x)*(x)
#define _y_dim(x)  (x)        
#define norm_R3(x) (sqrt((x[0])*(x[0])+(x[1])*(x[1])+(x[2])*(x[2])))
#define p1p3(x)    (sqrt((x[0])*(x[0])+(x[2])*(x[2])))     
#define _split(var,add,cond)	\
  do{                           \
      var -= cond;              \
      add++;	                \
  } while(var >= cond)

#define index_full(idf,dn,x)  ((idf) = x[0]+(dn)*x[1]+(dn)*(dn)*x[2])



static inline float
*eval_gradient(int dim_nod, int index[], float Du[], 
	       const float *func, float delta_x)
{
  register int i;

  int IF;
  float du_up,du_down;
  float *Du_tmp;

  Du_tmp = Du;

  for(i = 0; i < DIM_SPACE; i++){

    //  Approximation with finite diff centered :
    //    .) go up
    if(++index[i] > dim_nod -1)
      {
	--index[i];
	index_full(IF,dim_nod,index);
      }
    else
      {
	index_full(IF,dim_nod,index);
	--index[i];
      }
    
    du_up = func[IF];
    
    //    .) go down
    if(--index[i] < 0)
      {
	++index[i];
	index_full(IF,dim_nod,index);
      }
    else
      {
	index_full(IF,dim_nod,index);
	++index[i];
      }
    
    du_down = func[IF];

    //Initialize Du
    Du_tmp[i] = (du_up-du_down)/(2.0f*delta_x);
  }

  return Du_tmp;

}

static inline void 
eval_direction_vector(const float Du[],float ni[][NUM_VEC])
{
  
  // Define vector ni_1
  ni[0][0] = -Du[2]/p1p3(Du);
  ni[1][0] = 0;
  ni[2][0] = Du[0]/p1p3(Du);

  // Define vector n1_2
  ni[0][1] = -(Du[0]*Du[1])/(norm_R3(Du)*p1p3(Du));
  ni[1][1] = p1p3(Du)/norm_R3(Du);
  ni[2][1] = -(Du[1]*Du[2])/(norm_R3(Du)*p1p3(Du));

}


static inline float
mcm_above_threshold(const int index[], float ni[][NUM_VEC], const float *u_n,
	       float delta_t,const float *step,const float *first,
	       const float *last,int dim_nod, gridType g_nod)
{
  // Find the input point on the grid
  float x[DIM_SPACE];
  find_point(DIM_SPACE, index, g_nod, x);


  float I_n_point[DIM_SPACE];
  float rho = _step(delta_t);
  float u_mcm = 0.0f;
  
  //Direction (+ni1 +ni2)
  I_n_point[0] = x[0]+rho*(ni[0][0]+ni[0][1]);
  I_n_point[1] = x[1]+rho*(ni[1][0]+ni[1][1]);
  I_n_point[2] = x[2]+rho*(ni[2][0]+ni[2][1]);
  out_of_grid_check(DIM_SPACE,first,last,I_n_point);
  u_mcm += interpol_fun_discrete(DIM_SPACE,dim_nod,g_nod,I_n_point,
				 first,step,u_n);

  //Direction (-ni1 + ni2)
  I_n_point[0] = x[0]+rho*(-ni[0][0]+ni[0][1]);
  I_n_point[1] = x[1]+rho*(-ni[1][0]+ni[1][1]);
  I_n_point[2] = x[2]+rho*(-ni[2][0]+ni[2][1]);
  out_of_grid_check(DIM_SPACE,first,last,I_n_point);
  u_mcm += interpol_fun_discrete(DIM_SPACE,dim_nod,g_nod,I_n_point,
				 first,step,u_n);
  //Direction (+ni1 - ni2)
  I_n_point[0] = x[0]+rho*(ni[0][0]-ni[0][1]);
  I_n_point[1] = x[1]+rho*(ni[1][0]-ni[1][1]);
  I_n_point[2] = x[2]+rho*(ni[2][0]-ni[2][1]);
  out_of_grid_check(DIM_SPACE,first,last,I_n_point);
  u_mcm += interpol_fun_discrete(DIM_SPACE,dim_nod,g_nod,I_n_point,
				 first,step,u_n);
  //Direction (-ni1 - ni2)
  I_n_point[0] = x[0]+rho*(-ni[0][0]-ni[0][1]);
  I_n_point[1] = x[1]+rho*(-ni[1][0]-ni[1][1]);
  I_n_point[2] = x[2]+rho*(-ni[2][0]-ni[2][1]);
  out_of_grid_check(DIM_SPACE,first,last,I_n_point);
  u_mcm += interpol_fun_discrete(DIM_SPACE,dim_nod,g_nod,I_n_point,
				 first,step,u_n);
  u_mcm = (u_mcm)*(0.25f);

  return u_mcm;

 }


static inline float 
mcm_below_threshold(int *index, int dim_nod, const float *u_n)
{

  register int i;
  int IF;
  float u_mcm = 0.0f;

  for(i = 0; i < DIM_SPACE; i++){
    if(++index[i] > dim_nod -1)
      {
	--index[i];
	index_full(IF,dim_nod,index);
      }
    else
      {
	index_full(IF,dim_nod,index);
	--index[i];
      }
    
    u_mcm += u_n[IF];

    if(--index[i] < 0)
      {
	++index[i];
	index_full(IF,dim_nod,index);
      }
    else
      {
	index_full(IF,dim_nod,index);
	++index[i];
      }
    
    u_mcm += u_n[IF];
  }

  u_mcm = u_mcm/6.00f;
  
  return u_mcm;

}



void 
mcmschema(int dim_space,int grid_size,int dim_nod,float *u_n_plus_one, 
	      const float *u_n, const float *step,float delta_t,
	      gridType g_nod,const float *first,const float *last)
{

  register int i,j;

  // Install handler for SIGDIM

  struct sigaction sa,sa_def;

  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = handler_sigdim;
  sigaction(SIGDIM, &sa, &sa_def);
  
  if(dim_space != 3)
    raise(SIGDIM);
  
  // Reinstall the deafult action
  
  sigaction(SIGDIM, &sa_def, NULL);
  
  int index[DIM_SPACE]={0,0,0};
  float Du[DIM_SPACE];
  float ni[DIM_SPACE][NUM_VEC];

  
  for(i = 0; i < grid_size; i++){
    eval_gradient(dim_nod,index,Du,u_n,step[0]);
	if(norm_R3(Du) <= C*step[0] || p1p3(Du) <= C*step[0])
	  u_n_plus_one[i] = mcm_below_threshold(index,dim_nod,u_n);
	else
	  { 
	    eval_direction_vector(Du,ni);
	    u_n_plus_one[i] = mcm_above_threshold(index,ni,u_n,delta_t,step,
						  first,last,dim_nod,g_nod);
	  }
	index_cycle(j,DIM_SPACE,dim_nod,index);
    
  }
}
