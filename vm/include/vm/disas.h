#ifndef SILKVM_DISAS_H
#define SILKVM_DISAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "chunk.h"

void disassemble(Chunk*, int, const char*);

#ifdef __cplusplus
}
#endif

#endif