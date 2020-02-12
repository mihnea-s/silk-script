#ifndef SILKVM_PROGRAM_H
#define SILKVM_PROGRAM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "chunk.h"

typedef struct {
  Chunk* cnks;
  size_t len;
} Program;

void init_program(Program*);
void free_program(Program*);

#ifdef __cplusplus
}
#endif

#endif