#ifndef IMPLICITFORM_R
#define IMPLICITFORM_R

#include "node.r"

typedef float (*levelSet)(const void*,const float*);
typedef float (*volM)(const void*,const void*);
typedef float (*volerrM)(const void*, const float, const float);

struct __ImplicitForm {
  struct __Node _;
  struct __Object *delegate;
  levelSet u;
  volM vol;
  volerrM volErr;
  float level;
};

#endif // IMPLICITSURFACE_R
