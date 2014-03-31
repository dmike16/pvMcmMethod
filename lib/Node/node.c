#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include "node.r"
#include "node.h"

static void *nodes;

static void
*Node_new (const void *_self, va_list *app){
  struct __Node *result = cast(Node,super_new(Node,_self,app));

  result->next = nodes, nodes = result;
  return (void*) result;

}

static void
Node_delete(void *_self){
  struct __Node *self = cast(Node,_self);

  sunder(self);
  super_delete(Node,self);
}

static void
Node_reclaim(const void *_self, Method how){
  cast(Class,_self);

  while(nodes)
    how(nodes);
}

void
sunder(void *_self){
  struct __Node *self = cast(Node,_self);

  if(nodes == self)
    nodes = self->next;
  else if(nodes) {
      struct __Node *np = nodes;

      while(np->next && np->next != self)
        np = np->next;

      if(np->next)
        np->next = self->next;
    }
  self->next = 0;
}

const void *_Node;

const void
*__node(){
  return new(Class,"Node",Object,sizeof(struct __Node),
		  	 delete,"delete",Node_delete,
             new,"",Node_new,
             reclaim,"reclaim",Node_reclaim,
             (void*)0);
}
