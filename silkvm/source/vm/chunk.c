#include <stddef.h>
#include <stdint.h>

#include <chunk.h>
#include <rodata.h>
#include <mem.h>

void init_chunk(Chunk *cnk) {
  cnk->cap = 0;
  cnk->len = 0;
  cnk->codes = NULL;
  init_rodata(&cnk->rod);
}

void write_ins(Chunk *cnk, uint8_t ins) {
  if (cnk->len == cnk->cap) {
    size_t size = GROW_ARRAY(cnk->cap);
    cnk->codes = REALLOC_ARRAY(cnk->codes, uint8_t, cnk->cap, size);
    cnk->cap = size;
  }

  cnk->codes[cnk->len] = ins;
  cnk->len++;
}

size_t write_rod(Chunk *cnk, Value val) {
  rodata_write(&cnk->rod, val);
  return cnk->rod.len - 1;
}

void free_chunk(Chunk *cnk) {
  free_rodata(&cnk->rod);
  FREE_ARRAY(cnk->codes, uint8_t, cnk->cap);
}