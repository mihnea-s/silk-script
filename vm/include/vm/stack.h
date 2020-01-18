#ifndef SILKVM_STK_H
#define SILKVM_STK_H

#include <stddef.h>

#include "value.h"

#define SILKVM_STKSZ 256

typedef struct {
  size_t sz;
  size_t cap;
  Value *ptr;
  Value *sp;
} Stack;

void init_stk(Stack *stk);
void push_stk(Stack *stk, Value);
Value pop_stk(Stack *stk);
void reset_stk(Stack *stk);
void free_stk(Stack *stk);

#endif