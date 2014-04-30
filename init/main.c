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
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "grid.h"
#include "u0.h"
#include "u0_Torus.h"
#include "u_sphere.h"
#include "dumbell.h"
#include "interpol_fun.h"
#include "eval_ic_on_grid.h"
//#include "eval_method_errno.h"
#include "pvschema_core.h"
#include "vector_copy.h"
#include "heaviSide.h"

#define TOL 10E-07
#define _NLS 40
#define _check_time(a,b) if((a) > (b))(a)=(b)
#define pi 3.141592654f

#define _digits(x,y,k)				\
  k = x;  	     \
  y = 0;	     \
  do{   	     \
  ++y;               \
  }while((int)(k/=10))

#define _allocate_error(x)                                             \
do{                                                                    \
  if(x == NULL){                                                       \
    fprintf(stderr,"Error in allocate memory: %s\n",strerror(errno));  \
    exit(1);}                                                          \
 }while(0)
  
static int draw_flag = 0;

static int
spawn(char *prog,char **arg_list){
	pid_t child_pid;

// Duplicate the process
	child_pid = fork();

	if(child_pid != 0)
		return child_pid;
	else{
		execvp (prog,arg_list);
		fprintf(stderr,"An error occur in execvp\n");
		abort();
	}

	return 0;
}

static int
make_output_file(const float *buffer, char *name,int dimension)
{
  int fd,flag;

  fd = open (name,O_WRONLY | O_CREAT | O_TRUNC,0666);

  if(fd == -1){
    fprintf(stderr,"Erro in open file: %s\n",strerror(errno));
    return 1;
  }

  if((write(fd,buffer,sizeof(float)*dimension))==-1)
	  flag = -1;
  else
	  flag = 0;

  close(fd);

  return flag;

}

static float
*extractIC(const char *file, int grid_size){

	float *u0 = malloc(grid_size*sizeof(float));

	_allocate_error(u0);

	int  fd;
	if((fd=open(file,O_RDONLY,0666)) == -1){
		fprintf(stderr,"Error in open file: %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}

	if((read(fd,u0,sizeof(float)*grid_size)) == -1){
		close(fd);
		return NULL;
	}
	else {
		close(fd);
		return u0;
	}

}

static void
autogenerate_octave_script(char *default_name,char *ic_name,int dim_nod,
			   float *first, float *last, float level,int dim_space)
{
  register int i;
  int fd;
  struct iovec vec[_NLS];
  struct iovec *vec_next;
  char cwd[1024];

  if(getcwd(cwd,sizeof(cwd)) == NULL)
    fprintf(stderr,"Error in get the cur dir: %s\n",strerror(errno));
 
  vec_next = vec;


  vec_next->iov_base = "% This file is auto-generated by the main program\n";
  vec_next->iov_len = 50;
  ++vec_next;

  vec_next->iov_base = "% Don't change it\n";
  vec_next->iov_len = 18;
  ++vec_next;

  vec_next->iov_base = "clear\n";
  vec_next->iov_len = 6;
  ++vec_next;

  char *_open_1 = malloc((strlen(cwd)+33+1)*sizeof(char));
  _allocate_error(_open_1);
  strcpy(_open_1,"f1=fopen(\"");
  strcat(_open_1,cwd);
  strcat(_open_1,"/arch/axesNodes.dat");
  strcat(_open_1,"\");\n");
  vec_next->iov_base = _open_1;
  vec_next->iov_len = strlen(_open_1);
  ++vec_next;

  char *range;
  char *cycle_for;
  int nd,tmp;
  
 
  _digits(dim_nod,nd,tmp);
  range = malloc((nd+1)*sizeof(char)); 
  _allocate_error(range);
  sprintf(range,"%d",dim_nod);
  
  cycle_for = malloc((9+nd+1)*sizeof(char));
  _allocate_error(cycle_for);
  strcpy(cycle_for,"for i=1:");
  strcat(cycle_for,range);
  strcat(cycle_for,"\n");
  vec_next->iov_base = cycle_for;
  vec_next->iov_len = strlen(cycle_for);
  ++vec_next;

  vec_next->iov_base = "  x(i)=fscanf(f1,\"%f\",1);\n";
  vec_next->iov_len = 26;
  ++vec_next;

  vec_next->iov_base = "  y(i)=fscanf(f1,\"%f\",1);\n";
  vec_next->iov_len = 26;
  ++vec_next;

  vec_next->iov_base = "  z(i)=fscanf(f1,\"%f\",1);\n";
  vec_next->iov_len = 26;
  ++vec_next;

  vec_next->iov_base = "end\n";
  vec_next->iov_len = 4;
  ++vec_next;

  vec_next->iov_base = "fclose(f1);\n";
  vec_next->iov_len = 12;
  ++vec_next;

 char *_open_2;
 _open_2 = malloc((15+strlen(cwd)+strlen(default_name)+1)*sizeof(char));
 _allocate_error(_open_2);
 strcpy(_open_2,"f2=fopen(\"");
 strcat(_open_2,cwd);
 strcat(_open_2,"/");
 strcat(_open_2,default_name);
 strcat(_open_2,"\");\n");
 vec_next->iov_base = _open_2;
 vec_next->iov_len = strlen(_open_2);
 ++vec_next;

 char *_open_3 = NULL;

 if(!draw_flag) {
	 _open_3 = malloc((strlen(cwd)+strlen(ic_name)+15+1)*sizeof(char));
	 _allocate_error(_open_1);
	 strcpy(_open_3,"f3=fopen(\"");
	 strcat(_open_3,cwd);
	 strcat(_open_3,"/");
	 strcat(_open_3,ic_name);
	 strcat(_open_3,"\");\n");
 
	 vec_next->iov_base = _open_3;
	 vec_next->iov_len = strlen(_open_3);
	 ++vec_next;

	 vec_next->iov_base = "dataf3=fread(f3,inf,\"float\");\n";
	 vec_next->iov_len  =  30;
	 ++vec_next;
 }
 
 vec_next->iov_base = "dataf2=fread(f2,inf,\"float\");\n";
 vec_next->iov_len  =  30;
 ++vec_next;



 vec_next->iov_base = cycle_for;
 vec_next->iov_len = strlen(cycle_for);
 ++vec_next;
 
 char *cycle_for_j;
 
 cycle_for_j = malloc((11+nd+1)*sizeof(char));
 _allocate_error(cycle_for_j);
 strcpy(cycle_for_j,"  for j=1:");
 strcat(cycle_for_j,range);
 strcat(cycle_for_j,"\n");
 vec_next->iov_base = cycle_for_j;
 vec_next->iov_len = strlen(cycle_for_j);
 ++vec_next;
 
 char *cycle_for_k;
 
 cycle_for_k = malloc((13+nd+1)*sizeof(char));
 _allocate_error(cycle_for_k);
 strcpy(cycle_for_k,"    for k=1:");
 strcat(cycle_for_k,range);
 strcat(cycle_for_k,"\n");
 vec_next->iov_base = cycle_for_k;
 vec_next->iov_len = strlen(cycle_for_k);
 ++vec_next;
 
 char *id=malloc(30 + 2*strlen(range));
 _allocate_error(id);
 strcpy(id,"      id=k+(j-1)*");
 strcat(id,range);
 strcat(id,"+(i-1)*");
 strcat(id,range);
 strcat(id,"**2;\n");
 vec_next->iov_base = id;
 vec_next->iov_len = 29 + 2*strlen(range);
 ++vec_next;

 vec_next->iov_base = "      v(j,k,i)=dataf2(id);\n";
 vec_next->iov_len = 27;
 ++vec_next;
 
 if(!draw_flag){
	 vec_next->iov_base = "      u(j,k,i)=dataf3(id);\n";
	 vec_next->iov_len = 27;
	 ++vec_next;
 }
 
 vec_next->iov_base = "    end\n";
 vec_next->iov_len = 8;
 ++vec_next;
 
 vec_next->iov_base = "  end\n";
 vec_next->iov_len = 6;
 ++vec_next;
 
 vec_next->iov_base = "end\n";
 vec_next->iov_len = 4;
 ++vec_next;

 vec_next->iov_base = "fclose(f2);\n";
 vec_next->iov_len = 12;
 ++vec_next;

 if(!draw_flag){
	 vec_next->iov_base = "fclose(f3);\n";
	 vec_next->iov_len = 12;
	 ++vec_next;
 }
 
 vec_next->iov_base = "view(-38,20);\n";
 vec_next->iov_len = 14;
 ++vec_next;
 
 vec_next->iov_base = "[X,Y,Z]=meshgrid(x,y,z);\n";
 vec_next->iov_len = 25;
 ++vec_next;
 
 char *level_set, *isosurface_1=NULL;

 _digits((int)level,nd,tmp);
 level_set = malloc(nd+4+1);
 _allocate_error(level_set);
 sprintf(level_set,"%.3f",level);

 if(!draw_flag){

	 isosurface_1 = malloc(40+strlen(level_set)+1);
	 _allocate_error(isosurface_1);
	 strcpy(isosurface_1,"[faces,verts,c]=isosurface(X,Y,Z,u,");
	 strcat(isosurface_1,level_set);
	 strcat(isosurface_1,",Y);\n");

	 vec_next->iov_base = isosurface_1;
	 vec_next->iov_len = strlen(isosurface_1);
	 ++vec_next;
 }
 
 char *first_axis,*last_axis,*axis;
 
 _digits((int)first[0],nd,tmp);
 first_axis = malloc((nd+2)*sizeof(char));
 _allocate_error(first_axis);
 sprintf(first_axis,"%d",(int)first[0]);
 _digits((int)last[0],nd,tmp);
 last_axis = malloc((nd+2)*sizeof(char));
 _allocate_error(last_axis);
 sprintf(last_axis,"%d",(int)last[0]);
 
 axis = malloc((15+dim_space*(strlen(first_axis)+strlen(last_axis))+1)*sizeof(char));
 _allocate_error(axis);
  
 strcpy(axis,"axis([");
 for(i = 0; i < dim_space; i++){
   strcat(axis,first_axis);
   strcat(axis,",");
   strcat(axis,last_axis);
   if(i != dim_space-1)
     strcat(axis,",");
 }
 strcat(axis,"]);\n");


 char _patch[] = "p = patch(\"Faces\",faces,\"Vertices\",verts,\"FaceVertexCData\",c,...\n";
 char _patch_2[] = "\"FaceColor\",\"interp\",\"EdgeColor\",\"blue\");\n";

 if(!draw_flag){
	 vec_next->iov_base = axis;
	 vec_next->iov_len = strlen(axis);
	 ++vec_next;

	 vec_next->iov_base = _patch;
	 vec_next->iov_len = strlen(_patch);
	 ++vec_next;
  

	 vec_next->iov_base = _patch_2;
	 vec_next->iov_len = strlen(_patch_2);
	 ++vec_next;
 
	 vec_next->iov_base = "set(p,\"FaceLighting\",\"phong\");\n";
	 vec_next->iov_len = 31;
	 ++vec_next;
 
	 vec_next->iov_base = "figure()\n";
	 vec_next->iov_len = 9;
	 ++vec_next;

	 vec_next->iov_base = "view(-38,20);\n";
	 vec_next->iov_len = 14;
	 ++vec_next;
 }

 vec_next->iov_base = axis;
 vec_next->iov_len = strlen(axis);
 ++vec_next;

 char *isosurface_2;

 isosurface_2 = malloc(40+strlen(level_set)+1);
 _allocate_error(isosurface_2);
 strcpy(isosurface_2,"[faces,verts,c]=isosurface(X,Y,Z,v,");
 strcat(isosurface_2,level_set);
 strcat(isosurface_2,",Y);\n");

 vec_next->iov_base = isosurface_2;
 vec_next->iov_len = strlen(isosurface_2);
 ++vec_next;

 char _patch_3[] = "p = patch(\"Faces\",faces,\"Vertices\",verts,\"FaceVertexCData\",c,...\n";
 vec_next->iov_base = _patch_3;
 vec_next->iov_len = strlen(_patch_3);
 ++vec_next;

 char _patch_4[] = "\"FaceColor\",\"interp\",\"EdgeColor\",\"blue\");\n";
 vec_next->iov_base = _patch_4;
 vec_next->iov_len = strlen(_patch_4);
 ++vec_next;

 vec_next->iov_base = "set(p,\"FaceLighting\",\"phong\");\n";
 vec_next->iov_len = 31;
 ++vec_next;

 vec_next->iov_base = "pause";
 vec_next->iov_len = 5;
 ++vec_next;
 
 fd = open("scripts/plotSurface.m", O_WRONLY | O_CREAT | O_TRUNC, 0666);
 size_t byte_wrote;

 if(!draw_flag)
	 byte_wrote = writev(fd,vec,_NLS);
 else
	 byte_wrote = writev(fd,vec,_NLS-11);

 if((int)byte_wrote == -1){
   free(_open_2);
   free(cycle_for);
   free(cycle_for_j);
   free(cycle_for_k);
   free(axis);
   free(level_set);
   free(isosurface_1);
   free(isosurface_2);
   free(id);
   close(fd);
   fprintf(stdout,"ERROR in CREAT OCTAVE SCRIPT\n");
   exit(1);}

 close(fd); 

 free(range);
 free(first_axis);
 free(last_axis);
 free(_open_1);  
 free(_open_2);
 free(_open_3);
 free(cycle_for);
 free(cycle_for_j);
 free(cycle_for_k);
 free(level_set);
 free(isosurface_1);
 free(isosurface_2);
 free(axis);
 free(id);
  
}

static char
*update_bar(char *bar, int N,int iter,int tot_iter)
{
  register int i;
  char *update_bar = bar;
  int status = N*iter/tot_iter;


  for(i = 0; i < status; i++)
    update_bar[i] = '=';

  return update_bar;

}


float timeto,level,v0;

int 
main(int argc, char *argv[])
{
  
#ifdef MTRACE
  mtrace();
#endif /* MTRACE */

  clock_t start_clock = clock();
  
  gridType g_nod;
  register int i;
  char *ic_name = "arch/IC.dat";
  int dim_nod;
  int dim_space;
  float *first,*last,*step;
  float *nod_values;
  float time,radius[2];

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
  tmp = strtok(NULL,"\n");
  radius[0] = atof(tmp);
  //tmp = strtok(NULL,"\n");
  //radius[1] = atof(tmp);
  tmp = strtok(NULL,"\n");
  level = atof(tmp);
  first = (float*) malloc(dim_space*sizeof(float));
  last = (float*) malloc(dim_space*sizeof(float));
  for (i = 0; (tmp = strtok(NULL, "\n")) != NULL && i < dim_space;i++){

	first[i]=(float)atof(tmp);
    last[i] = (float)atof(tmp = strtok(NULL,"\n"));

  }

  free((void*)data);
  //End read from the file
  
  step = malloc(dim_space*sizeof(float));
  for (i = 0; i < dim_space; i++)
    step[i] = fabs(last[i] - first[i])/(dim_nod- 1.00f);
	
  // Print the values read from file
  fprintf(stdout,"********************************\n");
  fprintf(stdout,"Values read from %s :\n",argv[1]);
  fprintf(stdout,"Space R^%d\n""Number of nodes %d\n",dim_space,dim_nod);
  fprintf(stdout,"  °n        Axes Range         Spatial Step\n");
  for( i=0; i < dim_space; i++)
    fprintf(stdout," %3d      "      "  [%.2f,%.2f]        "
	    "DeltaX=%.2e\n",i+1,first[i],last[i],step[i]);
  fprintf(stdout,"TIMEOUT = %.2f\n",timeto);
  fprintf(stdout,"**********************************\n");
  //fprintf(stdout,"Initial Condition (IC): paraboloide with radius %.2f\n",radius);
  fprintf(stdout,"Level Set : %.2f\n",level);
  
  // Create the grid in R^n
  int grid_size = (int) pow(dim_nod,dim_space); 
  g_nod = create_grid(dim_nod,dim_space,first,step);
  fprintf(stdout,"Grid Size: %d\n",grid_size);

  if(argc==3){
	  ic_name = "arch/ICnoise.dat";
	  fprintf(stdout,"Initial Condition read from file: %s\n",argv[2]);
	  if(!(nod_values = extractIC(argv[2],grid_size))){
			  fprintf(stdout,"Error in Read file %s",argv[2]);
			  exit(EXIT_FAILURE);
	  }
  }
  else
  // Eval initial func on grid points	
	  nod_values = eval_ic_on_grid(grid_size,dim_space,dim_nod,g_nod,u_0,radius);
  	  //nod_values = eval_ic_on_grid(grid_size,dim_space,dim_nod,g_nod,u0_torus,radius);
  	  //nod_values = eval_ic_on_grid(grid_size,dim_space,dim_nod,g_nod,u0_dumbell,radius);

  // MCM method
  int child_status;
  char *arg_list[]={
		  "octave",
		  "scripts/plotSurface.m",
		  NULL
  };
  int tot_iter;	
  float delta_t = step[0];
  float *u_n_plus_one = malloc(grid_size*sizeof(float));
  float *u_n = malloc(grid_size*sizeof(float));
  char *default_name = NULL;
  char s;
  char *bar = malloc(18*sizeof(char));

  _allocate_error(bar);
  
  //Initialize u_n
  u_n = vector_copy(nod_values,u_n,grid_size);

  // Create file with axis nod and IC values
  output_axes_nod(g_nod,dim_space,"arch/axesNodes.dat");
  fprintf(stdout," FILE CREATED \n");
  if(argc != 3){
	  make_output_file(u_n,"arch/IC.dat",grid_size);
	  fprintf(stdout," FILE CREATED \n");
  }
  //Eval the solution pvmcm sphere and the exact collapse time
  //float *u_vp_sphere = malloc(grid_size*sizeof(float));

  fprintf(stdout,"Time to eval : %f\n",timeto);
  //fprintf(stdout,"The Sphere will collapse at the time: %.2f\n",
  //	  radius*radius/(2.00f*(dim_space-1)));

  // Eval the volume preserving constant for the sphere
  //float r0= extract_radius_sphere(radius,4,level);
  //float r0 = sqrt((*radius)*(*radius) -level);
  //fprintf(stdout,"r0=%f\n",r0);
  //v0=(4.00f/3.00f)*pi*powf(r0,3);
  //float vol_preserv = 2.00f*powf(4.00f*pi/(3.00f*v0),2.00f/3.00f);
  //float r0 = radius[0]*radius[0] +level;
  //v0 = (2.00f)*pi*pi*r0*radius[1];
  //v0 = 2.00f*powf(4.00f*pi/(3.00f*v0),2.00f/3.00f);
  //PVMCM method iteration
  float vf;
  float eps = 1.5*step[0];
  v0 = 0.00f;
  for(i=0;i<grid_size;i++)
       	v0 += 1-hvSide(level-u_n[i],eps);
  v0 *= powf(step[0],3);
  printf("V0 = %.2f\n",v0);
  do{
	vf = 0.00f;
    tot_iter =  timeto/delta_t;
    memset(bar,' ',18);
    bar[18] = '\0';
    i = 0;
    time = 0.00f;
    fprintf(stdout,"dt=%.2e\n",delta_t);
    for(;timeto;){
      ++i;
      if(tot_iter < i)++tot_iter;
      update_bar(bar,18,i,tot_iter);
      fprintf(stdout,"\r[%s]%d%%",bar,100*i/tot_iter);
      fflush(stdout);
      vpschema(dim_space,grid_size,dim_nod,u_n_plus_one,u_n,
		    step,delta_t,g_nod,first,last);

     // vpschema_core(dim_space,grid_size,dim_nod,u_n_plus_one,u_n,
     // 		    step,delta_t,g_nod,first,last,v0);

      time += delta_t;
      _check_time(time,timeto);
      if (time == timeto){
    	  default_name = "arch/dflMCMsolution.dat";
    	  if((make_output_file(u_n_plus_one,default_name,grid_size)) != -1)
    	  {
    		  fprintf(stdout,"\nTime %.2f reached in %d iter\n",timeto,i);
    		  fprintf(stdout,"FILE CREATED \n");
    	  }
    	  else
    		  perror("make_output_file");
    	  break;
      	  }
      	  u_n = vector_copy(u_n_plus_one,u_n,grid_size);
    	}


    // Case Output time = 0.0
    if(timeto == 0){
      fprintf(stdout,"Nothing to do the the solution is equal condition"
	      " to the initial condition\n");
      break;
    }

    //Eval the Norm infinity of the Error  
    //eval_method_errno(u_n_plus_one,u_vp_sphere,grid_size);
    for(i=0;i<grid_size;i++)
    	vf += 1-hvSide(level-u_n_plus_one[i],eps);

    printf("|%.2f -%.2f| = %e\n",v0,vf*powf(step[0],3),fabs(v0-(vf*powf(step[0],3))));
    //Generate octave script in order to plot the solution
    if(!draw_flag || draw_flag == 1)
    	autogenerate_octave_script(default_name,ic_name,dim_nod,first,last,level,dim_space);
    fprintf(stdout,"Script generated, into Dir \"scripts\"\n");
    fprintf(stdout,"Time used by CPU : %g sec.\n",(clock()-start_clock)/
	    (double) CLOCKS_PER_SEC);
    fprintf(stdout,"Do you want to plot the Solution(y/n)?\n");
    if((s=getchar())=='\n')
          s = getchar();
    if(s == 'y'){
    	/*
    	if((system("octave scripts/plotSurface.m")))
    		fprintf(stderr,"Erro in call Octave script\n");
    	else
    		++draw_flag;
    		*/
    	// Duplicate a process and execute the octave program
    	spawn("octave",arg_list);

    	// Wait untill the child process end
    	wait (&child_status);

    	// Check exit status of child process
    	if(WIFEXITED(child_status)){
    		if(WEXITSTATUS(child_status))
    			fprintf(stderr,"Error in call octave script\n");
    		else
    			++draw_flag;
    	}
    	else
    		fprintf(stderr,"Error in Call execvp function\n");


    }
    fprintf(stdout,"Do you want to helve delta T(y/n)?\n");
    if((s=getchar())=='\n')
      s = getchar();
    if(s == 'n')break;

    delta_t = delta_t/2.00f;
    u_n = vector_copy(nod_values,u_n,grid_size);
    
  }while(s != 'n' && (timeto));

  // Clean Allocated Memory
  clear_grid(g_nod,dim_space);
  free(step);
  free(first);
  free(last);
  free(nod_values);
  free(u_n);
  free(u_n_plus_one);
  //free(u_vp_sphere);
  free(bar);
    
  return 0;
  
}
