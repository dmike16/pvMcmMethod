/*
 * gargolyeIN.c
 *
 *  Created on: May 25, 2014
 *      Author: dmike
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/uio.h>
#include <errno.h>
#include "gargolyeIN.h"


float
*gargolyeIN(int grid_size,float step,float stepSTD)
{
	int fd;
	struct stat file_info;
	char *data,*token;
	size_t length;
	char *fileName = "./ic/IC-data/RICOS149_GARGOYLEsolo_N100";

	if((fd = open (fileName, O_RDONLY)) == -1){
			  fprintf(stderr,"Error in open file: %s\n",strerror(errno));
			  return NULL;
		  }
	fstat (fd,&file_info);
	length = file_info.st_size;

	data =  (char*) malloc (length);
	if((read (fd,data,length))== -1){
		fprintf(stderr,"Error in reading the file %s\n",fileName);
		close(fd);
		return NULL;
	}
	close(fd);

	float *u0;
	if((u0 = malloc(grid_size*sizeof(float)))==NULL)
	{
		fprintf(stderr,"Error in allocate memory: %s\n",strerror(errno));
		free(data);
		return NULL;
	}

	int i;
	token = strtok(data,"\n");
	float factor = (step*step*step)/stepSTD;
	u0[0] = 1.0f*factor*(atof(token));
	for(i = 1; i < grid_size && token != NULL; i++)
	{
		token = strtok(NULL,"\n");
		u0[i] = 1.0f*factor*(atof(token));
	}

	free(data);

	return u0;

}

