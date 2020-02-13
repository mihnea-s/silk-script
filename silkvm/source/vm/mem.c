#include <stdlib.h>

#include <mem.h>

void* memory(void* ptr, size_t old_sz, size_t new_sz) {
  if (new_sz == 0) {
    release(ptr, old_sz);
    return NULL;
  }

  return realloc(ptr, new_sz);
}

void release(void* ptr, size_t old_sz) {
  free(ptr);
}