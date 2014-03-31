#ifndef OBJECT_R
#define OBJECT_R

#ifndef Method_Type
#define Method_Type

typedef void (*Method) ();

#endif

typedef void * (*ctoR) (void*,va_list*);
typedef struct __Object * (*neW) (const void*,va_list*);
typedef void * (*dtoR) (void*);
typedef int (*putO)(const void*,FILE*);
typedef void (*reclM)(const void*,Method);

struct __Object {
       unsigned long magic;
       const struct __Class *class;
}; 

struct Method {
  const char *tag;
  Method selector;
  Method method;
};

struct __Class {
       const struct __Object _;
       const char *name;
       const struct __Class *super;
       size_t size;
       struct Method ctor;
       struct Method dtor;
       struct Method puto;
       struct Method delete;
       struct Method new;
       struct Method reclaim;
};

void *ctor(void* self, va_list *app);
void *dtor(void *self);
const void *classOf (const void *self);
const void *super (const void *self);
size_t sizeOf (const void *self);
void *super_ctor(const void *class, void *self, va_list *app);
void *super_dtor(const void *class, void *self);
void *super_new(const void *class, const void *self, va_list *app);
void super_delete(const void *class, void *self);
void super_reclaim(const void *class, const void *self, Method how);
int super_puto(const void *class,const void *self, FILE *fp);
int isA (const void *self, const struct __Class *class);
int isOf (const void *self, const struct __Class *class);
void *cast (const struct __Class *class,const void *self);


#endif /* header Object.h */
