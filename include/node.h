#ifndef NODE_H
#define NODE_H

#include "Object.h"

extern const void * _Node;
#define Node (_Node ? _Node : (_Node = __node()))

void sunder(void *self);
const void * __node();

#endif // NODE_H
