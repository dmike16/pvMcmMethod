#ifndef SURFACETORUS_H
#define SURFACETORUS_H

#include "surface.h"

extern const void* _SurfaceTorus;
#define SurfaceTorus (_SurfaceTorus ? _SurfaceTorus : \
  (_SurfaceTorus = __surfacetorus()))

const void* __surfacetorus();

#endif // SURFACETORUS_H
