#ifndef SILKVMEXE_READ_FILE_H
#define SILKVMEXE_READ_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "chunk.h"

Chunk* read_file(const char*, int*, const char**);
int    write_file(const char*, Chunk*, int, const char**);

#ifdef __cplusplus
}
#endif

#endif