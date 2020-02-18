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
  uint8_t* ra; // return address
  Value*   bp; // base pointer
} Frame;

typedef struct {
  Frame* ftop;                 // frame top
  Frame  farr[MOTHVM_FRM_CAP]; // frame array

  Value* vtop;                 // value top
  Value  varr[MOTHVM_STK_CAP]; // value array
} Stack;

void init_stk(Stack* stk);
void reset_stk(Stack* stk);

Value stk_top(Stack* stk);
Value stk_pop(Stack* stk);
void  stk_push(Stack* stk, Value);

Value stk_get(Stack* stk, size_t i);
void  stk_set(Stack* stk, size_t i, Value);

Frame*   stk_frame(Stack* stk);
void     stk_invoke(Stack* stk, uint8_t* ra, uint8_t argc);
uint8_t* stk_return(Stack* stk);

#ifdef __cplusplus
}
#endif

#endif