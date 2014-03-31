#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <math.h>
#include <assert.h>
#include "surfaceTorus.r"
#include "surfaceTorus.h"


static void
*SurfaceTorus_ctor(void *_self, va_list *app){
  struct __SurfaceTorus *self = super_ctor(SurfaceTorus,_self,app);

/* definition local, made becouse of the error check by qtcreator
 but the program works wtihout too
*/

  typedef double dprecision;

  self->r = va_arg(*app,dprecision);
  self->R = va_arg(*app,dprecision);

  return self;
}

static float
SurfaceTorus_surface(const void* _self, const float *x){
  struct __SurfaceTorus *self = cast(SurfaceTorus,_self);

  register int i;
  float u_xy = 0.00f, u_z = 0.00f;
  float r_two = (self->r)*(self->r);

    for (i = 0; i < Dim(self); i++){
      if(i != Dim(self)-1)
        u_xy += x[i]*x[i];
      else
        u_z = x[i]*x[i];
      }

    return -powf((self->R-sqrt(u_xy)),2) - u_z + r_two;

}




static float
SurfaceTorus_volError(const void* _self, const float vol, const float level){
  struct __SurfaceTorus *self = cast(SurfaceTorus,_self);

  printf("Surface Torus r=%.2f, R=%.2f\n",self->r,self->R);
  float r_zero = (self->r)*(self->r) + level;
  float vol_exact = (2.00f)*PI*PI*r_zero*self->R;

  return fabs(vol_exact - vol);
}


const void* _SurfaceTorus;

const void
*__surfacetorus(){
  return new(SurfaceClass,"SurfaceTorus",Surface,sizeof(struct __SurfaceTorus),
             ctor,"",SurfaceTorus_ctor,
             surface,"surface",SurfaceTorus_surface,
             volError,"volerror",SurfaceTorus_volError,
             (void*)0);
}

