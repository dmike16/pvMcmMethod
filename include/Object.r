#ifndef OBJECT_R
#define OBJECT_R

struct __Object {
       unsigned long magic;
       const struct __Class *class;
}; 

struct __Class {
       const struct __Object _;
       const char *name;
       const struct __Class *super;
       size_t size;
       void * (*ctor) (void *self,va_list *app);
       void * (*dtor) (void *self);
       int    (*puto) (const void *self, FILE *fp);
};

const void *classOf (const void *self);
const void *super (const void *self);
size_t sizeOf (const void *self);
void *super_ctor(const void *class, void *self, va_list *app);
void *super_dtor(const void *class, void *self);
int super_puto(const void *class,const void *self, FILE *fp);

#endif /* header Object.h */