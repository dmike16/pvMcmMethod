/* Name: grid_class.r
 * Pourpose: rapresentation file of grid struct and metaclass GridClass
 * Author: dmike
 */

#ifndef GRID_CLASS_R
#define GRID_CLASS_R

#include "Object.r"

struct __Grid {
       const struct __Object _;
       float **axes;
};

#define aaxes(p)	((const struct __Grid *)(p) -> axes)

// super method

struct __GridClass {
       const struct __Class _;
       //method
};

#endif /* rapresentation file of grid class */