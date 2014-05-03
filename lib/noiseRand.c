/*
 * noiseRand.c
 *
 *  Created on: 02/mag/2014
 *      Author: dmike
 *
 *   This function generate random noise
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include "grid.h"
#include "noiseRand.h"

static float
*randomNumber(int dim,float* first,float* last,float *ra_numb){
	register int j;

		for(j = 0; j < dim; j++)
			ra_numb[j] = first[0] +(last[0]-first[0])*(float)rand()/RAND_MAX;

	return ra_numb;
}

static void
randFree(int numb, void **ra_vec){
	int i;

	for(i = 0; i < numb; i++){
		free(ra_vec[i]);
	}

	free(ra_vec);
}

static int
makeNoise(int numb, int dim, const float **ra_center, const float *ra_radius,const float *x){
	register int i,j;
	float dist = 0;

	for(i = 0; i < numb; i++){
		for(j = 0; j < dim; j++)
			dist += (x[j]-ra_center[i][j])*(x[j]-ra_center[i][j]);
		if(dist <= ra_radius[i]*ra_radius[i])
			return 1;
		else
			dist = 0;
	}

	return 0;
}

float
*randNoise(int dim,gridType g,float* u,float* first, float* last,float nod){

	int i,j,numb;
	float *u_noise = u;
	float **ra_center;
	float *ra_radius;
	float sigma;

	fprintf(stdout,"Insert the mean noise value :");
	fscanf(stdin,"%f",&sigma);
	fprintf(stdout,"Insert the number of noise sphere you want to generate\n");
	fscanf(stdin,"%d",&numb);

	if(numb == 0){
		fprintf(stdout,"No noise add\n");
		return u_noise;
	}

	ra_center = malloc(numb*sizeof(float*));
	ra_radius = malloc(numb*sizeof(float));

	if(ra_center == NULL || ra_radius == NULL){
		fprintf(stderr,"Error in allocate memory %s",strerror(errno));
		return NULL;
	}

	// Create random small sphere center in the initial domain
	// and small random radius
	//
	srand((unsigned)time(NULL));
	float a = 0.0f,b = 1.0f;

	for(i = 0; i < numb; i++){
		ra_center[i] = malloc(dim*sizeof(float));

		if(ra_center[i] == NULL){
			fprintf(stdout,"Error in allocate memory %s\n",strerror(errno));
			abort();
		}

		randomNumber(dim,first,last,ra_center[i]);
		randomNumber(1,&a,&b,&ra_radius[i]);

	}

	int dim_n = (int)powf(nod,dim);
	int *id = calloc(dim,sizeof(int));
    float *x = malloc(dim*sizeof(float));

	if(id == NULL || x == NULL){
		fprintf(stdout,"Error in allocate memory %s\n",strerror(errno));
		abort();
	}

	for(i = 0; i < dim_n; i++){
		x = find_point(dim,id,g,x);
		if(makeNoise(numb,dim,(const float**)ra_center,ra_radius,x))
			u_noise[i] += sigma;
		index_cycle(j,dim,nod,id);
	}
	printf("Noise ADD\n");

	// Free all object allocated
	//
	randFree(numb,(void**)ra_center);
	free(ra_radius);
	free(x);
	free(id);

	return u_noise;
}

