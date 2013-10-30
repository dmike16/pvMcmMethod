/* Name: main.c
 * Pourpose: main program for the PVSMCM method
 * Author: dmike
 */

#ifdef MTRACE
#include <mcheck.h>
#endif /* MTRACE */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include "grid.h"
#include "u0.h"
#include "interpol_fun.h"
#include "eval_ic_on_grid.h"
#include "signal_dim.h"
#include "pvschema_core.h"
#include "vector_copy.h"

#define TOL 10E-07
#define _check_time(a,b) if((a) > (b))(a)=(b)

static int
make_output_file(const float *buffer, char *name,int dimension)
{
  int i;
  int fd;
  int char_numb = 0;

  fd = open (name,O_WRONLY | O_CREAT | O_TRUNC,0666);

  if(fd == -1){
    perror("open");
    return 1;
  }

  FILE *arg = fdopen(fd,"w");
  
   for(i = 0; i < dimension; ++i)
     char_numb = fprintf(arg,"%.6f\n",buffer[i]);

  fclose(arg);
  
  return char_numb;

}


int 
main(int argc, char *argv[])
{
  
#ifdef MTRACE
  mtrace();
#endif /* MTRACE */
  
  gridType g_nod;
  register int i;
  int dim_nod;
  int dim_space;
  float *first,*last,*step;
  float *nod_values,timeto;

  // Check the correct usage of the programm	
  if (argc != 2 && argc != 3){
    fprintf(stderr,"usage: pvschema filename.dat output_filename.dat\n");
	  exit(EXIT_FAILURE);
  }
  
  // Read from the file
  int fd;
  struct stat file_info;
  char *data,*tmp;
  size_t length;
  
  fd = open (argv[1], O_RDONLY);
  fstat (fd,&file_info);
  length = file_info.st_size;
  
  data =  (char*) malloc (length);
  if((read (fd,data,length))== -1)
    fprintf(stderr,"Error in reading the file %s\n",argv[1]);
  close(fd);
  
  tmp = strtok(data,"\n");
  timeto = atof(tmp);
  tmp = strtok(NULL, "\n");
  dim_space = atoi(tmp);
  tmp = strtok(NULL, "\n");
  dim_nod = atoi(tmp);
  first = (float*) malloc(dim_space*sizeof(float));
  last = (float*) malloc(dim_space*sizeof(float));
  for (i = 0; (tmp = strtok(NULL, "\n")) != NULL ;i++){
    first[i]=(float)atof(tmp);
    last[i] = (float)atof(tmp = strtok(NULL,"\n"));
  }
  
  free((void*)data);
  //End read from the file
  
  step = malloc(dim_space*sizeof(float));
  for (i = 0; i < dim_space; i++)
    step[i] = (last[i] - first[i])/(dim_nod - 1.00f);
	
  // Print the values read from file
  fprintf(stdout,"********************************\n");
  fprintf(stdout,"Values read from %s :\n",argv[1]);
  fprintf(stdout,"Space R^%d\n""Number of nodes %d\n",dim_space,dim_nod);
  fprintf(stdout,"  °n        Axes Range         Spatial Step\n");
  for( i=0; i < dim_space; i++)
    fprintf(stdout," %3d      "      "  [%.2f,%.2f]        "
	    "DeltaX=%.2f\n",i+1,first[i],last[i],step[i]);
  fprintf(stdout,"TIMEOUT = %.2f\n",timeto);
  fprintf(stdout,"**********************************\n");
  
  // Create the grid in R^n
  int grid_size = (int) pow(dim_nod,dim_space); 
  g_nod = create_grid(dim_nod,dim_space,first,step);
  
  // Eval initial func on grid points	
  nod_values = eval_ic_on_grid(grid_size,dim_space,dim_nod,g_nod,u_0);
  
  // Install handler for SIGDIM
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = handler_sigdim;
  sigaction(SIGDIM, &sa, NULL);
  
  // MCM method	
  float delta_t=step[0];
  float time = 0.0f;
  float *u_n_plus_one = malloc(grid_size*sizeof(float));
  float *u_n = malloc(grid_size*sizeof(float));
  
  u_n = vector_copy(nod_values,u_n,grid_size);
  fprintf(stdout,"grid_size %d\n",grid_size);
  
  output_axes_nod(g_nod,dim_space,"arch/axesNodes.dat");
  make_output_file(u_n,"arch/IC.dat",grid_size);
    
  i = 0;
  for(;timeto;){
    fprintf(stdout,".");
    fflush(stdout);
    time += delta_t;
    _check_time(time,timeto);
    ++i;
    pvschema_core(dim_space,grid_size,dim_nod,u_n_plus_one,u_n,
		  step,delta_t,g_nod,first,last);
    if (fabs(timeto-time) <= TOL){
      char *default_name;
      if(argc == 2)
	default_name = "arch/dflMCMsolution.dat";
      else
	default_name = argv[2];
      if((make_output_file(u_n_plus_one,default_name,grid_size)) != -1)
	{
	  fprintf(stdout,"\nTempo %.2f reached in %d iter\n",timeto,i);
	  fprintf(stdout,"FILE CREATED \n");
	}
      else
	perror("make_output_file");
      break;
    }
    u_n = vector_copy(u_n_plus_one,u_n,grid_size);
  }
     
    // Clean Allocated Memory
    clear_grid(g_nod,dim_space);
    free(step);
    free(first);
    free(last);
    free(nod_values);
    free(u_n);
    free(u_n_plus_one);
    
    return 0;

}
