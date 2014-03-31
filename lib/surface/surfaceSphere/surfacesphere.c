#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <math.h>
#include <assert.h>
#include "surfaceSphere.r"
#include "surfaceSphere.h"


static void
*SurfaceSphere_ctor(void *_self, va_list *app){
  struct __SurfaceSphere *self = super_ctor(SurfaceSphere,_self,app);

/* definition local, made becouse of the error check by qtcreator
 but the program works wtihout too
*/

  typedef double dprecision;

  self->radius = va_arg(*app,dprecision);

  return self;
}

static float
SurfaceSphere_surface(const void* _self, const float *x){
  struct __SurfaceSphere *self = cast(SurfaceSphere,_self);

  register int i;
  float u = 0.00f;
  float r_two = (self->radius)*(self->radius);


    for (i = 0; i < Dim(self); i++)
      u += (x[i])*(x[i]);
    if (u <= r_two)
      return r_two-u;
    else
     return 0.0f;
}



static float
SurfaceSphere_volError(const void* _self, const float vol, const float level){
  struct __SurfaceSphere *self = cast(SurfaceSphere,_self);

  float r_zero = sqrt((self->radius)*(self->radius) - level);
  float vol_exact = (4.00f/3.00f)*PI*powf(r_zero,Dim(self));

  return fabs(vol_exact - vol);
}


const void* _SurfaceSphere;

const void
*__surfacesphere(){
  return new(SurfaceClass,"SurfaceSphere",Surface,sizeof(struct __SurfaceSphere),
             ctor,"",SurfaceSphere_ctor,
             surface,"surface",SurfaceSphere_surface,
             volError,"volerror",SurfaceSphere_volError,
             (void*)0);
}
