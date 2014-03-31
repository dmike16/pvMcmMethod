#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <malloc.h>
#include <stdarg.h>
#include "implicitForm.r"
#include "implicitForm.h"


static void
*ImplicitForm_ctor(void *_self,va_list *app){
  struct __ImplicitForm *self = super_ctor(ImplicitForm,_self,app);

  typedef void* voidstar;
  typedef double dprecision;
  self->delegate = va_arg(*app,voidstar);
  self->level = va_arg(*app,dprecision);
  self->u = (levelSet)respondsTo(self->delegate,"surface");
  self->vol = (volM)respondsTo(self->delegate,"volume");
  self->volErr = (volerrM)respondsTo(self->delegate,"volerror");

  return self;
}

static void
*ImplicitForm_dtor (void *_self)
{
  struct __ImplicitForm *self = super_dtor(ImplicitForm,_self);

  free(dtor(self->delegate));

  return self;
}

float
uSurface(const void *_self, const float *x){
  struct __ImplicitForm *self = cast(Object,_self);

  if(self->u)
    return self->u(self->delegate,x);
  else {
      fprintf(stderr,"Error in find delegate implicit fuction definition: %s\n",strerror(errno));
      reclaim(self,delete);
      exit(1);
    }
}

float
uVol(const void *_self){
  struct __ImplicitForm *self = cast(Object,_self);
  if(self->vol)
    return self->vol(self->delegate,self);
  else {
      printf("### WARNING ###\n");
      printf("\t Method Volume not supported for this Oject\n");
      return -1.00f;
    }

}

float
uVolError(const void *_self,const float vol){
  struct __ImplicitForm *self = cast(Object,_self);
  if(self->volErr)
    return self->volErr(self->delegate,vol,self->level);
  else {
      printf("### WARNING ###\n");
      printf("\t Method Error Volume not supported for this Oject\n");
      return -1.00f;
    }

}

const void *_ImplicitForm;

const void
*__implicitform(){
  return new(Class,"ImplicitSurface",Object,sizeof(struct __ImplicitForm),
             ctor,"",ImplicitForm_ctor,
             dtor,"",ImplicitForm_dtor,
             (void*)0);
}
