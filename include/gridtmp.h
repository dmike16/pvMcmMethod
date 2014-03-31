#ifndef GRID_H
#define GRID_H

#include "Object.h"

extern const void *_GridClass;
extern const void *_Grid;

#define GridClass (_GridClass ? _GridClass : (_GridClass = __gridclass()))
#define Grid (_Grid ? _Grid : (_Grid = __grid()))

const void *__gridclass();
const void *__grid();

float *point(const void *self,const int *index,float *x);

#endif // GRID_H
