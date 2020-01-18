#ifndef SILKVM_CONSTANTS_H
#define SILKVM_CONSTANTS_H

#include <stddef.h>

#include "value.h"

typedef struct {
  size_t len;
  size_t cap;
  Value *vals;
} Constants;

void init_constants(Constants *);
void write_constant(Constants *, Value);
void free_constants(Constants *);

#endif