#ifndef SURFACE_R
#define SURFACE_R

#include "node.r"

typedef float * (*pointM) (const void*,const int*,float*);

struct __Surface {
  struct __Node _;
  struct __Object *delegate;
  int space;
  pointM point;
};

#define Delegate(x) (((const struct __Surface *) (x))->delegate)
#define Point(x) (((const struct __Surface *) (x))->point)
#define Dim(x) (((const struct __Surface *)(x))->space)

struct __SurfaceClass{
  struct __Class _;
  struct Method surface;
  struct Method volume;
  struct Method volError;
};

#endif // SURFACE_R
