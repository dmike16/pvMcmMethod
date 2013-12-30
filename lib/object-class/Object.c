/* Name: Object.c
 * Pourpose: describe two first class Object and Class
 * Author: dmike
 */

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>
#include "Object.r"
#include "Object.h"

#define MAGIC 0x0effaced
#define isObject(p) \
  (assert(p),       \
   assert(((struct __Object *) p)->magic == MAGIC),p)

static void 
*Object_ctor (void *_self,va_list *app)
{
  struct __Object *self = _self;
  
  return self;
}

static void
*Object_dtor (void *_self)
{
  struct __Object *self = _self;

  return self;
}

static int
Object_puto (const void *_self, FILE *fp)
{
  const struct __Class *class = classOf(_self);

  return fprintf(fp,"%s at %p\n",class->name,_self);
}

static void
*Class_dtor (void *_self)
{
  struct __Class *self = _self;
  
  fprintf(stderr, "%s: cannot destroy class \n",self->name);

  return 0;
}

static void
*dtor(void *_self)
{
  const struct __Class *class = classOf(_self);

  //check
  return class->dtor(_self);
}

static void 
*ctor(void *_self, va_list *app)
{
  const struct __Class *class = classOf(_self);

  //check
  return class->ctor(_self,app);
}

static void
*Class_ctor (void *_self,va_list *app)
{
  struct __Class *self = _self;

  self->name = va_arg(*app,char*);
  self->super = va_arg(*app,struct __Class *);
  self->size = va_arg(*app,size_t);

  //check 

  const size_t offset = offsetof(struct __Class, ctor);

  memcpy((char*) self + offset, (char*) self->super + offset,
	 sizeOf(self->super) - offset);

  typedef void (*voidf) ();
  voidf selector;
  va_list ap;

  va_copy(ap,*app);

  while((selector = va_arg(ap,voidf)))
    {
      voidf method = va_arg(ap,voidf);
      
      if(selector == (voidf) ctor)
	*(voidf*) & self -> ctor = method;
      else if(selector == (voidf) dtor)
	*(voidf*) & self -> dtor = method;
      else if(selector == (voidf) puto)
	*(voidf*) & self -> puto = method;
    }

  va_end(ap);

  return self;

}

static const struct __Class object[] = {
  {{MAGIC,object + 1},
   "Object",object,sizeof(struct __Object),
   Object_ctor,Object_dtor,Object_puto
  },
  {{MAGIC,object + 1},
   "Class",object,sizeof(struct __Class),
   Class_ctor,Class_dtor,Object_puto
  }
};

const void *_Object = object;
const void *_Class  = object + 1;

void
delete (void *_self)
{
  cast(_Object,_self);
  free(dtor(_self));
}

void
*new(const void *_self,...)
{
  const struct __Class *class = cast(_Class,_self);
  struct __Object *object;
  va_list ap;

  //check
  object = calloc(1,class->size);
  //check
  object->magic = MAGIC;
  object->class = class;
  va_start(ap,_self);
  object = ctor(object,&ap);
  va_end(ap);

  return object;
  
}

int 
puto(const void* _self, FILE *fp)
{
  int result;
  const struct __Class *class = classOf(_self);

  cast(_Object,_self);
  //check
  result = class->puto(_self,fp);

  return result;
}

const void
*classOf (const void *_self)
{
  const struct __Object *self = _self;
  // check

  return self->class;
}

const void
*super (const void *_self)
{
  const struct __Class *self = _self;
  //check
  
  return self->super;
}

size_t 
sizeOf (const void *_self)
{
  const struct __Class *class = classOf(_self);

  return class->size;
}

void 
*super_ctor(const void *_class, void *_self, va_list *app)
{
  const struct __Class *superclass = super(_class);

  //check

  return superclass->ctor(_self,app);
}

void
*super_dtor(const void *_class, void *_self)
{
  const struct __Class *superclass = super(_class);

  //check

  return superclass->dtor(_self);
}

int
super_puto(const void *_class, const void *_self, FILE *fp)
{
  int result;
  const struct __Class *superclass = super(_class);

  cast(_Object,_self);
  //check
  result = superclass->puto(_self,fp);

  return result;
}

int 
isA (const void *_self,const struct __Class *_class)
{
  return _self && classOf(_self) == _class;
}

int 
isOf (const void *_self, const struct __Class *_class)
{
  if(_self)
    {
      const struct __Class *myClass = classOf(_self);

      if(_class != _Object)
	while(myClass != _class)
	  if(myClass != _Object)
	    myClass = super(myClass);
	  else
	    return 0;
      return 1;
    }
  return 0;
}

static void
catch(int sig)
{
  assert(sig == 0);
}

void
*cast(const struct __Class *_class, const void *_self)
{
  void (*sigsegv)(int) = signal(SIGSEGV,catch);
#ifdef SIGBUS
  void (*sigbus)(int) = signal(SIGBUS, catch);
#endif
  const struct __Object *self = isObject(_self);
  const struct __Class *myClass = isObject(self->class);

  if(_class != _Object)
    {
      isObject(_class);
      while (myClass != _class)
	{
	  assert(myClass != _Object);
	  myClass = myClass->super;
	}
    }
#ifdef SIGBUS
  signal(SIGBUS,sigbus);
#endif
  signal(SIGSEGV,sigsegv);

  return (void*) self;
}
