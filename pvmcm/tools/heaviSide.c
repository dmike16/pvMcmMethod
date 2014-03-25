/*
 * heaviSide.c
 *
 *  Created on: Mar 24, 2014
 *      Author: dmike
 */

#include <math.h>
#include "heaviSide.h"

#define PI 3.14159265

float
hvSide(float x, float epsilon){
	if(x < -epsilon)
	    return 0.00f;
	  else if(x >= -epsilon && x <= epsilon)
	    return 0.50f + x/(2.00f*epsilon) + (1.00f/(2.00f*PI))*sin((PI*x)/epsilon);
	  else
	    return 1.00f;
}

float
delta_func(float x, float epsilon){
	if(x < -epsilon)
		return 0.00f;
	else if(x >= -epsilon && x <= epsilon)
		return 1.00f/(2.00f*epsilon)+ (1.00f/(2.00f*epsilon))*cos(PI*x/epsilon);
	else
		return 0.00f;
}


