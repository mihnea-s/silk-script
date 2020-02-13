#ifndef SILKVM_CONSTANTS_H
#define SILKVM_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "value.h"

typedef struct {
  size_t len;
  size_t cap;
  Value* vals;
} Rodata;

void init_rodata(Rodata*);
void rodata_write(Rodata*, Value);
void free_rodata(Rodata*);

#ifdef __cplusplus
}
#endif

#endif