#include <moth/garbage.h>

#include <moth/mem.h>
#include <moth/object.h>
#include <moth/value.h>

// The initial capacity of the GC's registry
#define GC_INIT_CAP 10

static void mark_object(Object *obj) {
  obj->reachable = true;

  switch (obj->type) {
    case O_STRING: {
      break;
    }

    case O_ARRAY: {
      ObjectArray *arr = OBJ_ARR(obj);

      for (Value *val = arr->vals; val < arr->vals + arr->size; val++) {
        if (val->type != T_OBJ) continue;
        mark_object(val->as.object);
      }

      break;
    }

    case O_VECTOR: {
      break;
    }

    case O_DICTIONARY: {
      ObjectDictionary *dict = OBJ_DCT(obj);

      for (size_t i = 0; i < dict->len; i++) {
        ObjectDictionaryEntry *entry = &dict->entries[i];
        if (entry->key.type == T_OBJ) mark_object(entry->key.as.object);
        if (entry->value.type == T_OBJ) mark_object(entry->value.as.object);
      }

      break;
    }

    case O_FUNCTION: {
      break;
    }

    case O_CLOSURE: {
      break;
    }

    case O_HEAPVAL: {
      ObjectHeapval *upv = OBJ_HPV(obj);

      if (upv->val.type == T_OBJ) { mark_object(upv->val.as.object); }

      break;
    }

    case O_FFI_FUNCTION: {
      break;
    }

    case O_FFI_POINTER: {
      break;
    }
  }
}

void init_gc(GarbageCollector *gc, Stack *stk) {
  gc->len  = 0;
  gc->cap  = GC_INIT_CAP;
  gc->stk  = stk;
  gc->objs = memory(NULL, 0, sizeof(Object *) * GC_INIT_CAP);
}

void gc_collect(GarbageCollector *gc) {
  // Mark all values on the VM's stack's value array
  for (Value *v = gc->stk->varr; v < gc->stk->vtop; v++) {
    if (v->type != T_OBJ) continue;
    mark_object(v->as.object);
  }

  // Free unreachable objects on the GC's registry
  for (Object **obj = gc->objs; obj < gc->objs + gc->len;) {
    if (!(*obj)->reachable) {
      free_object(*obj);
      gc->len--;
      *obj = *(gc->objs + gc->len);
    } else {
      (*obj)->reachable = false;
      obj++;
    }
  }
}

void gc_register(GarbageCollector *gc, Object *obj) {

  // Collect if the GC has reached max registry size
  if (gc->len == gc->cap) gc_collect(gc);

  // Grow capacity if the situation doesn't improve
  if (gc->len == gc->cap) {
    size_t new_cap = GROW_CAP(gc->cap);

    size_t new_size = sizeof(Object *) * new_cap;
    size_t old_size = sizeof(Object *) * gc->cap;

    gc->cap  = new_cap;
    gc->objs = memory(gc->objs, old_size, new_size);
  }

  // Register object for cleaning in the registry
  gc->objs[gc->len] = obj;
  gc->len++;
}

void free_gc(GarbageCollector *gc) {
  // Free all objects, we are done for today
  for (Object **obj = gc->objs; obj < gc->objs + gc->len; obj++) {
    free_object(*obj);
  }

  // ... and finally the object registry itself
  release(gc->objs, sizeof(Object *) * gc->cap);
}
