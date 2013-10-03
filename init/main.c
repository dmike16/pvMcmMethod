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
#include "grid.h"
#include "u0.h"
#include "interpol_fun.h"

int 
main(int argc, char *argv[])
{
	gridType g_nod,g_point;
	register int i;
	int dim_nod;
	int dim_space;
	float *first,*last,*step,*new_value;

#ifdef MTRACE
	mtrace();
#endif /* MTRACE */

	// Check the correct usage of the programm	
	if (argc != 2){
		fprintf(stderr,"usage: pvschema filename.dat\n");
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
	read (fd,data,length);
	close(fd);

	tmp = strtok(data,"\n");
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
	fprintf(stdout,"**********************************\n");

	// Create the grid in R^n and the stores the interpol points
	g_nod = create_grid(dim_nod,dim_space,first,step);
	g_point = interpol_points(dim_space,first,last);


	// Interpol the u0 function in the g_points grid
        new_value = interpol_fun(dim_space,g_nod,g_point,first,step);

	// Print the itnerpol value

	for(i = 0; i < g_point[0]->dim; i++)
	  fprintf(stdout," pt %d°, value = %f \n",i+1,new_value[i]);

	clear_grid(g_nod,dim_space);
	clear_grid(g_point,dim_space);
	free((void*)new_value);
	free((void*)step);
	free((void*)first);
	free((void*)last);

	return 0;

}
