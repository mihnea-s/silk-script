#include "value.h"
#include <stdint.h>
#include <string.h>

#include <mem.h>
#include <symtable.h>

void init_symtable(Symtable* stb, uint32_t init_len) {
  stb->len = init_len;
  stb->cap = init_len;
  stb->arr = init_len ? memory(NULL, 0, sizeof(Symbol) * init_len) : NULL;
}

void symtable_write(Symtable* stb, Symbol sym) {
  if (stb->cap == stb->len) {
    uint32_t new_cap = GROW_CAP(stb->cap);

    size_t new_size = sizeof(Symbol) * new_cap;
    size_t old_size = sizeof(Symbol) * stb->cap;

    stb->cap = new_cap;
    stb->arr = memory(stb->arr, old_size, new_size);
  }

  stb->arr[stb->len] = sym;
  stb->len++;
}

void free_symtable(Symtable* stb) {
  for (uint32_t i = 0; i < stb->len; i++) {
    release(stb->arr[i].str, strlen(stb->arr[i].str) + 1); // null byte
  }

  release(stb->arr, sizeof(Symbol) * stb->cap);
}
