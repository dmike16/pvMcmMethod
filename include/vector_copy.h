#ifndef VECTOR_COPY_H
#define VECTOR_COPY_H

inline float
*vector_copy(const float *vec1, float *vec2, int dim)
{
  const float *p_orig;
  float *p_copy;

  for(p_orig = vec1,p_copy = vec2; p_orig < vec1 + dim; p_orig++,p_copy++)
    *p_copy = *p_orig;

  return vec2;
}
#endif /* End vector_copy.h */
