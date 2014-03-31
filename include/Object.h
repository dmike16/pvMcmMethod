#ifndef OBJECT_H
#define OBJECT_H

#ifndef Method_Type
#define Method_Type

typedef void (*Method) ();

#endif

extern const void *_Object;
#define Object (_Object ? _Object : (_Object = __object()))

struct __Object *new (const void *class,...);
void *allocate(const void *self);
void delete (void *self);
void reclaim(const void *self, Method how);
int puto (const void *self,FILE *fp);
const void* __object();
const void* __class();
Method respondsTo(const void *self, const char *tag);

extern const void *_Class;
#define Class (_Class ? _Class : (_Class = __class()))

#endif /* header Object.h */
