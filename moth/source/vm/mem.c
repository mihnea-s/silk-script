#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mem.h>

typedef struct Alloc Alloc;

struct Alloc {
  Alloc* next;
  size_t size;
  void*  ptr;
};

static Alloc* freelist = NULL;

void* memory(void* ptr, size_t old_sz, size_t new_sz) {
  if (freelist) {
    Alloc* prev = NULL;
    Alloc* it   = freelist;

    while (it) {

      if (it->size == new_sz) {
        if (ptr) memcpy(it->ptr, ptr, old_sz);

        prev->next = it->next;
        return it->ptr;
      }

      prev = it;
      it   = it->next;
    }
  }

  void* ptrr = realloc(ptr, new_sz);
  return ptrr;
}

void cleanup() {
  while (freelist) {
    Alloc* next = freelist->next;
    free(freelist->ptr);
    free(freelist);
    freelist = next;
  }
}

void release(void* ptr, size_t size) {
  if (!ptr) return;
  if (!freelist) atexit(cleanup);

  size_t allc_sz = sizeof(Alloc);
  Alloc* allc    = malloc(allc_sz);
  allc->ptr      = ptr;
  allc->size     = size;
  allc->next     = freelist;
  freelist       = allc;
}
