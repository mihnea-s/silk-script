#ifndef SILKVM_CHUNK_H
#define SILKVM_CHUNK_H

#include <stddef.h>
#include <stdint.h>

#include "constants.h"

typedef struct {
  size_t len;
  size_t cap;
  uint8_t *codes;
  Constants constants;
} Chunk;

void init_chunk(Chunk *);
void write_ins(Chunk *, uint8_t);
size_t constant(Chunk *, Value);
void free_chunk(Chunk *);

#endif