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

static struct __Object
*Object_new (const void *_self, va_list *app){
  const struct __Class *self = cast(Class,_self);

  return ctor(allocate(self),app);
}

static void
Object_delete(void *_self){
  struct __Object *self=cast(Object,_self);

  free(dtor(self));
}

static void
Object_reclaim(const void *_self,Method how){
  cast(Object,_self);
  return;
}

static void
*Class_dtor (void *_self)
{
  struct __Class *self = _self;
  
  fprintf(stderr, "%s: cannot destroy class \n",self->name);

  return 0;
}

void
*dtor(void *_self)
{
  const struct __Class *cclass = cast(Class,classOf(_self));

  assert(cclass->dtor.method);
  return ((dtoR)cclass->dtor.method)(_self);
}

void
*ctor(void *_self, va_list *app)
{
  const struct __Class *cclass = cast(Class,classOf(_self));

  assert(cclass->ctor.method);

  return ((ctoR)cclass->ctor.method)(_self,app);
}

void
*allocate(const void *_self)
{
  struct __Object *object;
  struct __Class *self = cast(Class,_self);

  assert(self->size);
  object = calloc(1,self->size);

  assert(object);
  object->magic = MAGIC;
  object->class = self;

  return object;
}

static void
*Class_ctor (void *_self,va_list *app)
{
  struct __Class *self = _self;

  self->name = va_arg(*app,char*);
  self->super = va_arg(*app,struct __Class *);
  self->size = va_arg(*app,size_t);

  assert(self->super);

  const size_t offset = offsetof(struct __Class, ctor);

  memcpy((char*) self + offset, (char*) self->super + offset,
	 sizeOf(self->super) - offset);

  Method selector;

  va_list ap;

  va_copy(ap,*app);

  while((selector = va_arg(ap,Method)))
    {
      const char *tag = va_arg(ap,const char *);
      Method method = va_arg(ap,Method);
      
      if(selector == (Method) ctor){
          if(tag)
            self->ctor.tag = tag;
          self->ctor.selector = selector;
          self->ctor.method = method;
          continue;
        }
      if(selector == (Method) dtor){
          if(tag)
            self->dtor.tag = tag;
          self->dtor.selector = selector;
          self->dtor.method = method;
          continue;
        }
      if(selector == (Method) puto){
          if(tag)
            self->puto.tag = tag;
          self->puto.selector = selector;
          self->puto.method = method;
          continue;
        }
      if(selector == (Method) delete){
          if(tag)
            self->delete.tag = tag;
          self->delete.selector = selector;
          self->delete.method = method;
          continue;
        }
      if(selector == (Method) new){
          if(tag)
            self->new.tag = tag;
          self->new.selector = selector;
          self->new.method = method;
          continue;
        }
      if(selector == (Method) reclaim){
          if(tag)
            self->reclaim.tag = tag;
          self->reclaim.selector = selector;
          self->reclaim.method = method;
          continue;
        }
    }

  va_end(ap);

  return self;

}

static const struct __Class object []={
  {{MAGIC,object+1},
   "Object",object,sizeof(struct __Object),
  {"", (Method) 0, (Method)Object_ctor},
  {"", (Method) 0, (Method)Object_dtor},
  {"puto", (Method) puto, (Method)Object_puto},
   {"delete", (Method) delete, (Method) Object_delete},
   {"", (Method) 0, (Method) Object_new},
   {"reclaim", (Method) 0, (Method) Object_reclaim}
  },
  {{MAGIC,object+1},
   "Class",object,sizeof(struct __Class),
   {"", (Method) 0, (Method) Class_ctor},
   {"", (Method) 0, (Method) Class_dtor},
   {"puto", (Method) puto, (Method) Object_puto},
   {"delete", (Method) delete, (Method)Object_delete},
   {"", (Method) 0, (Method)Object_new},
   {"reclaim", (Method) 0, (Method) Object_reclaim}
  }
};

const void *_Object = object;
const void *_Class  = object + 1;

const void
*__object(){
  return _Object;
}

const void
*__class(){
  return _Class;
}

void
delete (void *_self)
{
  struct __Class *self = cast(Class,classOf(_self));

  assert(self->delete.method);
  ((dtoR) self->delete.method)(_self);
}

struct __Object
*new(const void *_self,...)
{
  struct __Object *result;
  va_list ap;
  const struct __Class *self = cast(Class,_self);

  assert(self->new.method);
  va_start(ap,_self);
  result = ((neW) self->new.method)(_self, &ap);

  va_end(ap);
  return result;
}

void
reclaim (const void *_self, Method how)
{
  struct __Class *self = cast(Class,classOf(_self));

  assert(self->reclaim.method);
  ((reclM) self->reclaim.method)(_self,how);
}

void
*super_new(const void *_class, const void *_self, va_list *app){
  const struct __Class *superclass = super(_class);

  assert(superclass->new.method);
  return ((neW) superclass->new.method)(_self,app);
}

void
super_delete(const void *_class, void *_self){
  const struct __Class *superclass = super(_class);

  assert(superclass->delete.method);
  ((dtoR) superclass->delete.method)(_self);
}

void
super_reclaim(const void *_class, const void *_self, Method how){
  const struct __Class *superclass = super(_class);

  assert(superclass->delete.method);
  ((reclM) superclass->reclaim.method)(_self,how);
}

int 
puto(const void* _self, FILE *fp)
{

  const struct __Class *cclass = cast(Class,classOf(_self));
  int result;

  assert(cclass->puto.method);
  result = ((putO)cclass->puto.method)(_self,fp);

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
  const struct __Class *self = cast(Class,_self);
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

  assert(superclass->ctor.method);
  return ((ctoR)superclass->ctor.method)(_self,app);
  //check

}

void
*super_dtor(const void *_class, void *_self)
{
  const struct __Class *superclass = super(_class);

  assert(superclass->dtor.method);
  return ((dtoR)superclass->dtor.method)(_self);

}

int
super_puto(const void *_class, const void *_self, FILE *fp)
{
  int result;
  const struct __Class *superclass = super(_class);

  assert(superclass->puto.method);
  result = ((putO)superclass->puto.method)(_self,fp);

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

      if(_class != Object)
	while(myClass != _class)
	  if(myClass != Object)
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

  if(_class != Object)
    {
      isObject(_class);
      while (myClass != _class)
	{
	  assert(myClass != Object);
	  myClass = myClass->super;
	}
    }
#ifdef SIGBUS
  signal(SIGBUS,sigbus);
#endif
  signal(SIGSEGV,sigsegv);

  return (void*) self;
}

Method
respondsTo(const void *_self, const char *tag){
  if(tag && *tag){
      const struct __Class * cclass = classOf(_self);
      const struct Method *p = & cclass->ctor; // it must be the first;
      int nmeth = (sizeOf(cclass)-offsetof(struct __Class, ctor))/sizeof(struct Method);
      do {
          if(p->tag && strcmp(p->tag,tag) == 0)
            return p->method ? p->selector : 0;
        }while(++p,--nmeth);
    }
  return 0;
}
