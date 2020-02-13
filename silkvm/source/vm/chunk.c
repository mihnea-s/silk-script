#include <stddef.h>
#include <stdint.h>

#include <chunk.h>
#include <constants.h>
#include <mem.h>

void init_chunk(Chunk *cnk) {
  cnk->cap = 0;
  cnk->len = 0;
  cnk->codes = NULL;

  Constants cnst;
  init_constants(&cnst);
  cnk->constants = cnst;
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

size_t constant(Chunk *cnk, Value val) {
  write_constant(&cnk->constants, val);
  return cnk->constants.len - 1;
}

void free_chunk(Chunk *cnk) {
  FREE_ARRAY(cnk->codes, uint8_t, cnk->cap);
  free_constants(&cnk->constants);
}