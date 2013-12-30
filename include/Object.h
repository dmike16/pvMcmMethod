#ifndef OBJECT_H
#define OBJECT_H

extern const void *_Object;

void *new (const void *class,...);
void delete (void *self);
int puto (const void *self,FILE *fp);
int isA (const void *self, const struct __Class *class);
int isOf (const void *self, const struct __Class *class);
void *cast (const struct __Class *class,const void *self);

extern const void *_Class;

#endif /* header Object.h */
