#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include "grid.r"
#include "surface.r"
#include "gridtmp.h"

static void
*Grid_ctor(void *_self, va_list *app){
  struct __Grid *self = super_ctor(Grid,_self,app);

  typedef int integer;
  typedef float** floatstarstar;
  self->dim = va_arg(*app,integer);
  self->steps = va_arg(*app,integer);
  float **Omega = va_arg(*app,floatstarstar);

  register int i,j;

  self->delatax = malloc(sizeof(float)*self->dim);
  self->axes = malloc(sizeof(float*)*self->dim);
  assert(self->axes);

  for(i = 0; i < self->dim; i++){
      self->delatax[i]=fabs(Omega[i][1]-Omega[i][0])/(self->steps-1.00f);
    self->axes[i] = malloc(sizeof(float)*self->steps);
    assert(self->axes[i]);
    for(j = 0; j < self->steps; j++)
      self->axes[i][j] = Omega[i][0] + j*self->delatax[i];
    }

  return self;
}

static void
*Grid_dtor (void *_self)
{
  struct __Grid *self = super_dtor(Grid,_self);

  register int i;

  for(i = 0; i < self->dim; i++){
      free(self->axes[i]);
    }
  free(self->axes);
  free(self->delatax);

  return self;
}

static float
*Grid_point(const void* _self, const int *index, float *x){
  struct __Grid *self = cast(Grid,_self);

  register int i;
  float *p = x;

  for(i = 0; i < self->dim; i++)
    p[i] = self->axes[i][index[i]];

  return p;

}

static void
*GridClass_ctor(void *_self, va_list *app){
  struct __GridClass *self = super_ctor(GridClass,_self,app);

  typedef const char *stringconst;
  Method selector;
  va_list ap;

  va_copy(ap,*app);

  while((selector = va_arg(ap,Method))){
      const char *tag = va_arg(ap,stringconst);

      Method method = va_arg(ap,Method);

// Install function  point
      if(selector == (Method) point){
          if(tag)
            self->point.tag = tag;
          self->point.selector = selector;
          self->point.method = method;
          continue;
        }
    }

  va_end(ap);
  return self;
}


float
*point(const void *_self, const int *index, float *x){
  struct __GridClass *self = cast(GridClass,classOf(_self));

  assert((pointM)point == point);
  assert(self->point.method);

  return ((pointM)self->point.method)(_self,index,x);
}

const void *_GridClass;
const void *_Grid;

const void
*__gridclass(){
  return new(Class,"GridClass",Class,sizeof(struct __GridClass),
             ctor,"",GridClass_ctor,
             (void*)0);
}

const void
*__grid(){
  return new(GridClass,"Grid",Object,sizeof(struct __Grid),
             ctor,"",Grid_ctor,
             dtor,"",Grid_dtor,
             point,"point",Grid_point,
             (void*)0);
}
