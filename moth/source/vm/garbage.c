#include <garbage.h>
#include <mem.h>
#include <object.h>
#include <value.h>

#define GC_INIT_CAP 10

void init_gc(GarbageCollector* gc, Stack* stk) {
  gc->len  = 0;
  gc->cap  = GC_INIT_CAP;
  gc->stk  = stk;
  gc->objs = memory(NULL, 0, sizeof(Object*) * GC_INIT_CAP);
}

void gc_collect(GarbageCollector* gc) {
  // collection time
}

void gc_register(GarbageCollector* gc, Object* obj) {
  if (gc->len == gc->cap) {
    gc_collect(gc);

    if (gc->len == gc->cap) {
      size_t new_cap = GROW_CAP(gc->cap);

      size_t new_size = sizeof(Object*) * new_cap;
      size_t old_size = sizeof(Object*) * gc->cap;

      gc->cap  = new_cap;
      gc->objs = memory(gc->objs, old_size, new_size);
    }
  }

  gc->objs[gc->len] = obj;
  gc->len++;
}

void free_gc(GarbageCollector* gc) {
  for (Object** obj = gc->objs; obj < gc->objs + gc->len; obj++) {
    free_object(*obj);
  }

  release(gc->objs, sizeof(Object*) * gc->cap);
}
