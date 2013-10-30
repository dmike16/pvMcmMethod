/* Name: eval_direction_vector.c
 * Pourpose: approximate the vector v_1 and v_2 , autovector of the projection 
 * matrix P(p)=I-(pp^t)/(|p|^2).
 * Author: dmike
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
#include "grid.h"
#include "interpol_fun.h"
#include "pvschema_core.h"
#include "signal_dim.h"

#define DIM_SPACE  3
#define NUM_VEC    2
#define TOL        10E-03
#define _step(x)   sqrt(2.0f*(x))
#define _z_dim(x)  (x)*(x)
#define _y_dim(x)  (x)        
#define norm_R3(x) (sqrt((x[0])*(x[0])+(x[1])*(x[1])+(x[2])*(x[2])))
#define p1p3(x)    (sqrt((x[0])*(x[0])+(x[2])*(x[2])))     
#define _split(var,add,cond)	\
  do{                           \
      var -= cond;              \
      add++;	                \
  } while(var >= cond)

static inline int
index_full(int dim_nod,int index[])
{
  int index_full;

  index_full = index[0]+dim_nod*index[1]+dim_nod*dim_nod*index[2];

  return index_full;
}

static inline int
*index_split(int id, int index[], int dim_nod)
{
  int flag,range;
  // Index of type (0,0,id_z)  
  if(id >= 0 && id <= _y_dim(dim_nod)-1)
    index[0]=id,index[1]=0,index[2]=0;
  // Index of type (0,id_y,id_z) 
  else if(id >= _y_dim(dim_nod) && id <= _z_dim(dim_nod)-1){
    index[2] = 0;
    flag = 0;
    range = _y_dim(dim_nod);
    _split(id,flag,range);
    index[1] = flag;
    index[0] = id;
  }
  // Index of type (id_x,id_y,id_z)
  else {
    flag = 0;
    range = _z_dim(dim_nod);
    _split(id,flag,range);
    index[2] = flag;
    if(id >= 0 && id < _y_dim(dim_nod))
      index[1] = 0,index[0] = id;
    else
      {
	flag = 0;
	range = _y_dim(dim_nod);
	_split(id,flag,range);
	index[1] = flag;
	index[0] = id;
      }
    
  }
  return index;
}


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
    ++index[i];
    if(index[i] > dim_nod-1){
      --index[i];
      IF = index_full(dim_nod,index);
    }
    else{
      IF = index_full(dim_nod,index);
      --index[i];
    }  
    du_up = func[IF];

    //    .) go down
    --index[i];
    if(index[i] < 0){
      ++index[i];
      IF = index_full(dim_nod,index);
    }
    else {
      IF = index_full(dim_nod,index);
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
  ni[0][0] = -Du[2]/(p1p3(Du)+TOL);
  ni[1][0] = 0;
  ni[2][0] = Du[1]/(p1p3(Du)+TOL);

  // Define vector n1_2
  ni[0][1] = -(Du[0]*Du[1])/(norm_R3(Du)*p1p3(Du)+TOL);
  ni[1][1] = p1p3(Du)/(norm_R3(Du)+TOL);
  ni[2][1] = -(Du[1]*Du[2])/(norm_R3(Du)*p1p3(Du)+TOL);

}

static inline void
out_of_boundary_check(const float *first,const float *last, float pt[])
{
  register int i;

  for(i = 0; i < DIM_SPACE; i++)
    if(pt[i] < first[i])
      pt[i] = first[i];
    else if(pt[i] > last[i])
      pt[i] = last[i];

}

static inline float
mcm_iteraction(const int index[], float ni[][NUM_VEC], const float *u_n,
	       float delta_t,const float *step,const float *first,
	       const float *last,int dim_nod, gridType g_nod)
{
  // Find the input point on the grid
  float x[DIM_SPACE];
  find_point(DIM_SPACE, index, g_nod, x);


  float I_n_point[DIM_SPACE];
  float rho = sqrt(2.00f*delta_t);
  float u_mcm = 0;
  
  //Direction (+ni1 +ni2)
  I_n_point[0] = x[0]+rho*(ni[0][0]+ni[0][1]);
  I_n_point[1] = x[1]+rho*(ni[1][0]+ni[1][1]);
  I_n_point[2] = x[2]+rho*(ni[2][0]+ni[2][1]);
  out_of_boundary_check(first,last,I_n_point);
  u_mcm += interpol_fun_discrete(DIM_SPACE,dim_nod,g_nod,I_n_point,
				 first,step,u_n);
  //Direction (-ni1 + ni2)
  I_n_point[0] = x[0]+rho*(-ni[0][0]+ni[0][1]);
  I_n_point[1] = x[1]+rho*(-ni[1][0]+ni[1][1]);
  I_n_point[2] = x[2]+rho*(-ni[2][0]+ni[2][1]);
  out_of_boundary_check(first,last,I_n_point);
  u_mcm += interpol_fun_discrete(DIM_SPACE,dim_nod,g_nod,I_n_point,
				 first,step,u_n);
  //Direction (+ni1 - ni2)
  I_n_point[0] = x[0]+rho*(ni[0][0]-ni[0][1]);
  I_n_point[1] = x[1]+rho*(ni[1][0]-ni[1][1]);
  I_n_point[2] = x[2]+rho*(ni[2][0]-ni[2][1]);
  out_of_boundary_check(first,last,I_n_point);
  u_mcm += interpol_fun_discrete(DIM_SPACE,dim_nod,g_nod,I_n_point,
				 first,step,u_n);
  //Direction (-ni1 - ni2)
  I_n_point[0] = x[0]+rho*(-ni[0][0]-ni[0][1]);
  I_n_point[1] = x[1]+rho*(-ni[1][0]-ni[1][1]);
  I_n_point[2] = x[2]+rho*(-ni[2][0]-ni[2][1]);
  out_of_boundary_check(first,last,I_n_point);
  u_mcm += interpol_fun_discrete(DIM_SPACE,dim_nod,g_nod,I_n_point,
				 first,step,u_n);
  u_mcm = (u_mcm)*(0.25f);

  return u_mcm;

 }


void 
pvschema_core(int dim_space,int grid_size,int dim_nod,float *u_n_plus_one, 
	      const float *u_n, const float *step,float delta_t,
	      gridType g_nod,const float *first,const float *last)
{

  register int i;

  if(dim_space != 3)
    raise(SIGDIM);
  
  int index[DIM_SPACE];
  float Du[DIM_SPACE];
  float ni[DIM_SPACE][NUM_VEC];

  
  for(i = 0; i < grid_size; i++){
    index_split(i,index,dim_nod);
    eval_gradient(dim_nod,index,Du,u_n,step[0]);
    eval_direction_vector(Du,ni);
    u_n_plus_one[i] = mcm_iteraction(index,ni,u_n,delta_t,step,first,
				     last,dim_nod,g_nod);
  
  }
}
