#ifndef SURFACESPHERE_H
#define SURFACESPHERE_H

#include "surface.h"

extern const void* _SurfaceSphere;
#define SurfaceSphere (_SurfaceSphere ? _SurfaceSphere : \
  (_SurfaceSphere = __surfacesphere()))

const void* __surfacesphere();
#endif // SURFACESPHERE_H
