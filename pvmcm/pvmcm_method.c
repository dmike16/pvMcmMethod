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
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "grid.h"
#include "interpol_fun.h"
#include "pvschema_core.h"
#include "signal_dim.h"
#include "out_boundary.h"
#include "out_of_grid_check.h"
#include "heaviSide.h"


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

#define CHECK_OPEN(x) \
  if((x) == -1)       \
    { perror("open"); \
      abort();}

#define __CLOSE_THE_STRING(x)				\
  do{ char *__skip_until_the_last = data;		\
    while(__skip_until_the_last < data+write_byte-1)	\
      ++__skip_until_the_last;				\
    *__skip_until_the_last='\0';}while(0)   				

//static char digits[35];

#define index_full(idf,dn,x)  ((idf) = x[0]+(dn)*x[1]+(dn)*(dn)*x[2])



/*
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
*/

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
    index_full(IF,dim_nod,index);
    --index[i];
    
    du_up = func[IF];
    
    //    .) go down
    --index[i];
    index_full(IF,dim_nod,index);
    ++index[i];
    
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
pvmcm_above_threshold(const int index[], float ni[][NUM_VEC], const float *u_n,
	       float delta_t,const float *step,const float *first,
	       const float *last,int dim_nod, gridType g_nod, float I_n)
{
  // Find the input point on the grid
  float x[DIM_SPACE];
  find_point(DIM_SPACE, index, g_nod, x);


  float I_n_point[DIM_SPACE];
  float rho = _step(delta_t);
  float u_mcm = 0.0f;
    
  
  //Direction (+ni1 +ni2)
  I_n_point[0] = x[0]*(1-I_n*delta_t)+rho*(ni[0][0]+ni[0][1]);
  I_n_point[1] = x[1]*(1-I_n*delta_t)+rho*(ni[1][0]+ni[1][1]);
  I_n_point[2] = x[2]*(1-I_n*delta_t)+rho*(ni[2][0]+ni[2][1]);
  out_of_grid_check(DIM_SPACE,first,last,I_n_point);
  u_mcm += interpol_fun_discrete(DIM_SPACE,dim_nod,g_nod,I_n_point,
				 first,step,u_n);

  //Direction (-ni1 + ni2)
  I_n_point[0] = x[0]*(1-I_n*delta_t)+rho*(-ni[0][0]+ni[0][1]);
  I_n_point[1] = x[1]*(1-I_n*delta_t)+rho*(-ni[1][0]+ni[1][1]);
  I_n_point[2] = x[2]*(1-I_n*delta_t)+rho*(-ni[2][0]+ni[2][1]);
  out_of_grid_check(DIM_SPACE,first,last,I_n_point);
  u_mcm += interpol_fun_discrete(DIM_SPACE,dim_nod,g_nod,I_n_point,
				 first,step,u_n);
  //Direction (+ni1 - ni2)
  I_n_point[0] = x[0]*(1-I_n*delta_t)+rho*(ni[0][0]-ni[0][1]);
  I_n_point[1] = x[1]*(1-I_n*delta_t)+rho*(ni[1][0]-ni[1][1]);
  I_n_point[2] = x[2]*(1-I_n*delta_t)+rho*(ni[2][0]-ni[2][1]);
  out_of_grid_check(DIM_SPACE,first,last,I_n_point);
  u_mcm += interpol_fun_discrete(DIM_SPACE,dim_nod,g_nod,I_n_point,
				 first,step,u_n);
  //Direction (-ni1 - ni2)
  I_n_point[0] = x[0]*(1-I_n*delta_t)+rho*(-ni[0][0]-ni[0][1]);
  I_n_point[1] = x[1]*(1-I_n*delta_t)+rho*(-ni[1][0]-ni[1][1]);
  I_n_point[2] = x[2]*(1-I_n*delta_t)+rho*(-ni[2][0]-ni[2][1]);
  out_of_grid_check(DIM_SPACE,first,last,I_n_point);
  u_mcm += interpol_fun_discrete(DIM_SPACE,dim_nod,g_nod,I_n_point,
				 first,step,u_n);
  u_mcm = (u_mcm)*(0.25f);

  return u_mcm;

 }

/*static inline float
pvmcm_advanction(const int index[], const float *u_n,const float *first,
	       const float *last, const float *step,float delta_t,
	       float C_v, int dim_nod, gridType g_nod)
{
  // Find the input point on the grid
  float x[DIM_SPACE];
  find_point(DIM_SPACE, index, g_nod, x);


  float I_n_point[DIM_SPACE];

  I_n_point[0] = x[0]*(1.00f-C_v*delta_t);
  I_n_point[1] = x[1]*(1.00f-C_v*delta_t);
  I_n_point[2] = x[2]*(1.00f-C_v*delta_t);
  out_of_grid_check(DIM_SPACE,first,last,I_n_point);

  return interpol_fun_discrete(DIM_SPACE,dim_nod,g_nod,I_n_point,
				 first,step,u_n);



 }
*/
/*static inline char
*extract_triple_index(char *ptr, int *index)
{ int i;

  if( *ptr == '\0')
    return ptr;
  else 
    { for(i = 0; i < DIM_SPACE; i++)
	{ errno = 0;
	  index[i] = (int)strtol(ptr,&ptr,10);
	  // fprintf(stdout,"[%d]= %d\n",i,index[i]);
	  if(errno == ERANGE)
	    fprintf(stdout,"Invalid conversion\n");
	}
      return ptr;
    }
}
*/
static inline float
*pvmcm_below_threshold(int data, size_t wb,int *index, int dim_nod, float *u_new)
{

  register int i;
  int IF;
  float u_mcm = 0.0f;
  size_t br;
  /*char *ptr;
  ptr=(char*)data;
  
  while(*ptr != '\0')
    { ptr = extract_triple_index(ptr,index);
      for(i = 0; i < DIM_SPACE; i++){
	++index[i];
	index_full(IF,dim_nod,index);
	--index[i];

	u_mcm += u_new[IF];
	    
	--index[i];
	index_full(IF,dim_nod,index);
	++index[i];
	
	u_mcm += u_new[IF];
      }
      
      index_full(IF,dim_nod,index);
      u_new[IF] = u_mcm/6.00f;
      u_mcm = 0.0f;
    }*/
  while(wb){
	  if((signed)(br = read(data,index,sizeof(int)*DIM_SPACE))==-1){
		  perror("read");
		  abort();
	  }
	  wb -= br;
	  for(i = 0; i < DIM_SPACE; i++){
	  	++index[i];
	  	index_full(IF,dim_nod,index);
	  	--index[i];

	  	u_mcm += u_new[IF];

	  	--index[i];
	  	index_full(IF,dim_nod,index);
	  	++index[i];

	  	u_mcm += u_new[IF];
	   }

	   index_full(IF,dim_nod,index);
	   u_new[IF] = u_mcm/6.00f;
	   u_mcm = 0.0f;
  }
  return u_new;
}
 
static inline float
mcm_below_threshold(int *index, int dim_nod,const float *u_n)
{

  register int i;
  int IF;
  float u_mcm = 0.0f;


  for(i = 0; i < DIM_SPACE; i++){
	++index[i];
	index_full(IF,dim_nod,index);
	--index[i];

	u_mcm += u_n[IF];

	--index[i];
	index_full(IF,dim_nod,index);
	++index[i];

	u_mcm += u_n[IF];
  }

  return u_mcm;
}

static inline void
vpschema_core(int grid_size,int dim_nod,float *u_n_plus_one,
	      const float *u_n, const float *step,float delta_t,
	      gridType g_nod,const float *first,const float *last, float I_n)
{

  register int i,j;

  /* Install handler for SIGDIM

  struct sigaction sa,sa_def;

  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = handler_sigdim;
  sigaction(SIGDIM, &sa, &sa_def);

  if(dim_space != 3)
    raise(SIGDIM);

   Reinstall the default action

  sigaction(SIGDIM, &sa_def, NULL);
*/

  int index[DIM_SPACE] = {0,0,0};
  int fd;
  float Du[DIM_SPACE];
  float ni[DIM_SPACE][NUM_VEC];

  fd = open("index-tmp.bin",O_RDWR | O_CREAT | O_APPEND, 0666);
  CHECK_OPEN(fd);
  unlink("index-tmp.bin");

  size_t write_byte = 0;

  for(i = 0; i < grid_size; i++){
    //index_split(i,index,dim_nod);
    if((out_boundary(DIM_SPACE,dim_nod,index)))
     {
	eval_gradient(dim_nod,index,Du,u_n,step[0]);
	if(norm_R3(Du) <= C*step[0] || p1p3(Du) <= C*step[0])
	  { u_n_plus_one[i] = u_n[i];
	    //sprintf(digits,"%d %d %d\n",index[0],index[1],index[2]);
	    //write_byte += write(fd, digits, strlen(digits));
	  	write_byte += write(fd, index, sizeof(int)*DIM_SPACE);
		//u_n_plus_one[i] = mcm_below_threshold(index,dim_nod,u_n);
	  }

	else
	  {
	    eval_direction_vector(Du,ni);
	    u_n_plus_one[i] = pvmcm_above_threshold(index,ni,u_n,delta_t,step,
						  first,last,dim_nod,g_nod,I_n);
	  }
     }
    else
      u_n_plus_one[i] = u_n[i];

    index_cycle(j,DIM_SPACE,dim_nod,index);

  }


  // If there are some points below the threshold we use an "ad hoc" method
  if(write_byte != 0)
    {

      lseek(fd,0,SEEK_SET);
      /*
      char *data = malloc(write_byte+1);

      if(data == NULL)
	{ perror("malloc");
	  abort();
	}

      if((int)(read(fd,data,write_byte))== -1)
	{ perror("read");
	  exit(1);
	}
      __CLOSE_THE_STRING(data);
      */
      u_n_plus_one = pvmcm_below_threshold(fd,write_byte,index,dim_nod,u_n_plus_one);

      //free(data);
    }
  close(fd);
}

void 
vpschema(int dim_space,int grid_size,int dim_nod,float *u_n_plus_one,
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
  
  // Reinstall the default action
  
  sigaction(SIGDIM, &sa_def, NULL);
  
  int index[DIM_SPACE]={0,0,0};
  //int fd;
  float Du[DIM_SPACE];
  float ni[DIM_SPACE][NUM_VEC];
  float eps = step[0]*1.5f;
  float I_n = 0.0f;
  //float C_v = 0.00f;
  //float vol1 = 0.0f,vol2 = 0.0f;
  float* w = malloc(grid_size*sizeof(float));
/*
  fd = open("index-tmp.txt",O_RDWR | O_CREAT | O_APPEND, 0666);
  CHECK_OPEN(fd);
  unlink("index-tmp.txt");

   size_t write_byte = 0;
*/
  for(i = 0; i < grid_size; i++){
    //index_split(i,index,dim_nod);
    if((out_boundary(DIM_SPACE,dim_nod,index)))
     {
	eval_gradient(dim_nod,index,Du,u_n,step[0]);
	if(norm_R3(Du) <= C*step[0] || p1p3(Du) <= C*step[0])
	  { //w[i] = u_n[i];
	    //sprintf(digits,"%d %d %d\n",index[0],index[1],index[2]);
	    //write_byte += write(fd, digits, strlen(digits));
		w[i] = (mcm_below_threshold(index,dim_nod,u_n)-6.00f*u_n[i])*(1/(step[0]*step[0]));

	  }
				
	else
	  { 
	    eval_direction_vector(Du,ni);
	    w[i] = (mcm_above_threshold(index,ni,u_n,delta_t,step,
						  first,last,dim_nod,g_nod) -u_n[i])*(1/delta_t);

	  }
     }
    else
      w[i] = u_n[i];
    
    I_n += (w[i]*delta_func((level-u_n[i]),eps));

   //vol1 += 1-hvSide(level-u_n[i],eps);
   //vol2 += 1-hvSide(level-w[i],eps);
   index_cycle(j,DIM_SPACE,dim_nod,index);

  }

 /*
  if(write_byte != 0)
      {
        lseek(fd,0,SEEK_SET);
        char *data = malloc(write_byte+1);

        if(data == NULL)
  	{ perror("malloc");
  	  abort();
  	}

        if((int)(read(fd,data,write_byte))== -1)
  	{ perror("read");
  	  exit(1);
  	}
        __CLOSE_THE_STRING(data);
        w = pvmcm_below_threshold(data,index,dim_nod,w);

        free(data);
      }
    close(fd);
*/
 // for (i = 0; i < grid_size; i++){
	  //I_n = (w[i]*delta_func(level-u_n[i],eps));
	  //vol1 += 1-hvSide(level-u_n[i],eps);
	  //vol2 += 1-hvSide(level-w[i],eps);
	  //v0 += 1-hvSide(level-u_n[i],eps);
  //}

  I_n = -(I_n * powf(step[0],DIM_SPACE))/(3.00f*v0);
  //printf("\niint_S(H dS)/3V0= %.3f\n",I_n);
  //vol1 *= powf(step[0],DIM_SPACE);
  //vol2 *= powf(step[0],DIM_SPACE);
  //printf("\nVol1 = %.2f , Vol2 = %.2f \n",vol1,vol2);
  //C_v = (vol1-vol2);
  /*
  for(i = 0; i < grid_size; i++){
      index_split(i,index,dim_nod);
      if((out_boundary(DIM_SPACE,dim_nod,index)))
         u_n_plus_one[i] = pvmcm_advanction(index,w,first,last,
  	    		step,delta_t,C_v,dim_nod,g_nod);
      else
        u_n_plus_one[i] = w[i];

    }
*/
free(w);

vpschema_core(grid_size,dim_nod,u_n_plus_one,u_n,
      		    step,delta_t,g_nod,first,last,I_n);

}
