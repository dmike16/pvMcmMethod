#ifndef IMPLICITFORM_H
#define IMPLICITFORM_H

#include "Object.h"

extern const void* _ImplicitForm;
#define ImplicitForm (_ImplicitForm ? _ImplicitForm : \
  (_ImplicitForm = __implicitform()))

float uSurface(const void *self, const float *x);
float uVol(const void *self);
float uVolError(const void *self,const float vol);
const void* __implicitform();

#endif // IMPLICITSURFACE_H
