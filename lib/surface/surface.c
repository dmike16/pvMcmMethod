#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <assert.h>
#include <math.h>
#include "surface.r"
#include "implicitForm.r"
#include "grid.r"
#include "surface.h"


static void
*Surface_ctor(void *_self, va_list *app){
  struct __Surface *self = super_ctor(Surface,_self,app);

  typedef int integer;
  typedef void* voidstar;
  self->delegate = va_arg(*app,voidstar);
  self->space = va_arg(*app,integer);
  self->point = (pointM) respondsTo(self->delegate,"point");

  return self;
}

static void
*Surface_dtor (void *_self)
{
  struct __Surface *self = super_dtor(Surface,_self);

  free(dtor(self->delegate));

  return self;
}

static float
Surface_volume(const void *_self,const void *_implicitSurf){

  struct __Surface *self = cast(Surface,_self);
  struct __ImplicitForm *impSurf = cast(Object,_implicitSurf);

  float deltax = Deltax(self->delegate);
  float epsilon = deltax*(3.00f/2.00f);
  int steps = Steps(self->delegate);
  int gSize = (int)powf(steps,self->space);
  float vol = 0.0f;
  register int i,flag;
  float *x = malloc(sizeof(float)*self->space);
  int *index = calloc(1,sizeof(int)*self->space);

  assert(x);
  assert(index);


  for(i = 0; i < gSize; i++){
      x = ((pointM)self->point)(self->delegate,index,x);
      vol += (1-heaviSide((impSurf->level)-(impSurf->u)(self,x),epsilon));

      for(flag = 0; flag < self->space; flag++)
            if(index[flag] == steps-1)
              index[flag] = 0;
            else{
              ++index[flag];
              break;}
    }

  free(x);
  free(index);

  return vol*powf(deltax,self->space);
}


static void
*SurfaceClass_ctor(void *_self, va_list *app){
  struct __SurfaceClass *self = super_ctor(SurfaceClass,_self,app);

  typedef const char *stringconst;
  Method selector;
  va_list ap;

  va_copy(ap,*app);

  while((selector = va_arg(ap,Method))){
      const char *tag = va_arg(ap,stringconst);

      Method method = va_arg(ap,Method);

// Install function  implicit
      if(selector == (Method) surface){
          if(tag)
            self->surface.tag = tag;
          self->surface.selector = selector;
          self->surface.method = method;
          continue;
        }
// Install function eval volume
      if(selector == (Method) volume){
          if(tag)
            self->volume.tag = tag;
          self->volume.selector = selector;
          self->volume.method = method;
          continue;
        }
// Install error method
      if(selector == (Method) volError){
          if(tag)
            self->volError.tag = tag;
          self->volError.selector = selector;
          self->volError.method = method;
          continue;
        }
    }

  va_end(ap);
  return self;
}

float
surface(const void *_self,const  float *x){
  struct __SurfaceClass *self = cast(SurfaceClass,classOf(_self));

  assert((levelSet)surface == surface);
  assert(self->surface.method);

  return ((levelSet)self->surface.method)(_self,x);
}

float
volume(const void *_self, const void *implicitSurf){
  struct __SurfaceClass *self = cast(SurfaceClass,classOf(_self));

  assert((volM)volume == volume);
  assert(self->volume.method);

  return ((volM)self->volume.method)(_self,implicitSurf);
}

float
volError(const void *_self, const float vol, const float level){
  struct __SurfaceClass *self = cast(SurfaceClass,classOf(_self));

  assert((volerrM) volError == volError);
  assert(self->volError.method);

  return ((volerrM)self->volError.method)(_self,vol,level);
}

float
heaviSide(float x, float epsilon){
  if(x < -epsilon)
    return 0.00f;
  else if(x >= -epsilon && x <= epsilon)
    return 0.50f + x/(2.00f*epsilon) + (1.00f/(2.00f*PI))*sin((PI*x)/epsilon);
  else
    return 1.00f;
}

const void *_SurfaceClass;
const void *_Surface;

const void*
__surfaceclass(){
  return new(Class,"SurfaceClass",Class,sizeof(struct __SurfaceClass),
             ctor,"",SurfaceClass_ctor,
             (void*)0);
}

const void*
__surface(){
  return new(SurfaceClass,"Surface",Object,sizeof(struct __Surface),
             ctor,"",Surface_ctor,
             dtor,"",Surface_dtor,
             volume,"volume",Surface_volume,
             (void*)0);
}
