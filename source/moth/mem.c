#include <moth/mem.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM_STACK_SIZE 2048

static char  stack[MEM_STACK_SIZE];
static void *top = stack;

static const void *const bottom = stack + MEM_STACK_SIZE;

void *memory(void *ptr, size_t old_sz, size_t new_sz) {
  // Allocate nothing
  if (new_sz == 0) {
    release(ptr, old_sz);
    return NULL;
  }

  // Use malloc if the stack is full, or the new allocation
  // is over 32 bytes
  if (top == bottom || new_sz > 32) {
    // Release the stack storage
    if ((void *)stack <= ptr && ptr < bottom) {
      release(ptr, old_sz);
      ptr = NULL;
    }

    return realloc(ptr, new_sz);
  }

  void *new_ptr = top;
  top += (new_sz > 8) ? new_sz : 8;

  memcpy(new_ptr, ptr, old_sz);
  release(ptr, old_sz);

  return new_ptr;
}

void release(void *ptr, size_t size) {
  // If the value is allocated outside of the memory stack
  if (ptr < (void *)stack || bottom < ptr) return free(ptr);

  // TODO: deallocate memory on the Stack
  // if (stack <= (char*)ptr && (char*)ptr <= top) {
  //   if (ptr == top) top -= size + size % sizeof(size_t);
  //   return;
  // }
}
