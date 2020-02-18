#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mem.h>

typedef struct Alloc Alloc;

static char  stack[2048];
static char* top = stack;

void* memory(void* ptr, size_t old_sz, size_t new_sz) {
  void* nptr = NULL;

  if (new_sz < 24) {
    nptr = top;
    top += new_sz + new_sz % sizeof(size_t);
    if (nptr) memcpy(nptr, ptr, old_sz);
  } else {
    nptr = realloc(ptr, new_sz);
    ptr  = NULL;
  }

  if (ptr) release(ptr, old_sz);
  return nptr;
}

void release(void* ptr, size_t size) {
  if (stack <= (char*)ptr && (char*)ptr <= top) {
    if (ptr == top) top -= size + size % sizeof(size_t);
    return;
  }

  free(ptr);
}
