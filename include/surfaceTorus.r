#ifndef SURFACETORUS_R
#define SURFACETORUS_R

#include "surface.r"

struct __SurfaceTorus {
  struct __Surface _;
  float r;
  float R;
};

#endif // SURFACETORUS_R
