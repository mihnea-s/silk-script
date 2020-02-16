#include <stdint.h>
#include <string.h>

#include <mem.h>
#include <symtable.h>

void init_symtable(Symtable* stb, uint32_t init_len) {
  stb->len  = init_len;
  stb->cap  = init_len;
  stb->syms = NULL;
  if (init_len) stb->syms = memory(stb->syms, 0x0, stb->len);
}

void symtable_write(Symtable* stb, Symbol sym) {
  if (stb->cap == stb->len) {
    uint32_t new_size = GROW_ARRAY(stb->cap);
    stb->syms         = REALLOC_ARRAY(stb->syms, Symbol, stb->cap, new_size);
    stb->cap          = new_size;
  }

  stb->syms[stb->len] = sym;
  stb->len++;
}

void free_symtable(Symtable* stb) {
  for (uint32_t i = 0; i < stb->len; i++) {
    release(stb->syms[i].str, strlen(stb->syms[i].str) + 1);
  }

  FREE_ARRAY(stb->syms, Symbol, stb->cap);
}
