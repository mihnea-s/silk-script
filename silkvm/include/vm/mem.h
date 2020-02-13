#ifndef SILKVM_MEM_H
#define SILKVM_MEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#define GROW_ARRAY(SIZE) (SIZE < 4) ? 4 : SIZE * 2.0

#define REALLOC_ARRAY(PTR, TYPE, OLD, NEW)                                     \
  (TYPE*)memory(PTR, OLD * sizeof(TYPE), NEW * sizeof(TYPE))

#define FREE_ARRAY(PTR, TYPE, SIZE) memory(PTR, SIZE * sizeof(TYPE), 0)

void* memory(void*, size_t, size_t);

#ifdef __cplusplus
}
#endif

#endif