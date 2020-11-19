#ifndef MOTHVM_GARBAGE_H
#define MOTHVM_GARBAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <moth/object.h>
#include <moth/stack.h>

typedef struct {
  size_t   len;
  size_t   cap;
  Stack*   stk;
  Object** objs;
} GarbageCollector;

void init_gc(GarbageCollector* gc, Stack* stk);
void gc_collect(GarbageCollector* gc);
void gc_register(GarbageCollector* gc, Object* obj);
void free_gc(GarbageCollector* gc);

#ifdef __cplusplus
}
#endif

#endif