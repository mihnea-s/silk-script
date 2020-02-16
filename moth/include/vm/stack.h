#ifndef MOTHVM_STK_H
#define MOTHVM_STK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "value.h"

#define MOTHVM_FRM_CAP 256
#define MOTHVM_STK_CAP 1024

typedef struct {
  uint8_t* rt;
  Value*   bp;
} Invokation;

typedef struct {
  Invokation* itop;
  Invokation  iarr[MOTHVM_FRM_CAP];

  Value* vtop;
  Value  varr[MOTHVM_STK_CAP];
} Stack;

void init_stk(Stack* stk);

Value top_stk(Stack* stk);
Value pop_stk(Stack* stk);
void  push_stk(Stack* stk, Value);

Invokation* frame_stk(Stack* stk);
uint8_t*    return_stk(Stack* stk);
void        invoke_stk(Stack* stk, uint8_t* rt);

Value get_stk_local(Stack* stk, size_t i);
void  set_stk_local(Stack* stk, size_t i, Value);

void reset_stk(Stack* stk);
void free_stk(Stack* stk);

#ifdef __cplusplus
}
#endif

#endif