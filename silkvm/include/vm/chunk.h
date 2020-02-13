#ifndef SILKVM_CHUNK_H
#define SILKVM_CHUNK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "rodata.h"

typedef struct {
  size_t    len;
  size_t    cap;
  Rodata rod;
  uint8_t*  codes;
} Chunk;

void   init_chunk(Chunk*);
void   write_ins(Chunk*, uint8_t);
size_t write_rod(Chunk*, Value);
void   free_chunk(Chunk*);

#ifdef __cplusplus
}
#endif

#endif