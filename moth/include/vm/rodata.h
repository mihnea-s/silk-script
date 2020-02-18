#ifndef MOTHVM_RODATA_H
#define MOTHVM_RODATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "value.h"

typedef struct {
  uint32_t len;
  uint32_t cap;
  Value*   arr;
} Rodata;

void init_rodata(Rodata*, uint32_t);
void rodata_write(Rodata*, Value);
void free_rodata(Rodata*);

#ifdef __cplusplus
}
#endif

#endif