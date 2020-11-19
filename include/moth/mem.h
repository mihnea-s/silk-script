#ifndef MOTHVM_MEM_H
#define MOTHVM_MEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#define GROW_CAP(OLD_CAP) ((OLD_CAP < 4) ? 4 : OLD_CAP * 2.0)

void* memory(void*, size_t, size_t);
void  release(void*, size_t);

#ifdef __cplusplus
}
#endif

#endif