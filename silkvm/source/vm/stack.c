#include <mem.h>
#include <stack.h>
#include <value.h>

void init_stk(Stack* stk) {
  stk->sz  = 0;
  stk->cap = SILKVM_STKSZ;
  stk->ptr = REALLOC_ARRAY(stk->ptr, Value, 0, SILKVM_STKSZ);
  stk->sp  = stk->ptr;
}

void push_stk(Stack* stk, Value val) {
  if (stk->sz == stk->cap) {
    size_t size = GROW_ARRAY(stk->cap);
    stk->ptr    = REALLOC_ARRAY(stk->ptr, Value, stk->cap, size);
    stk->cap    = size;
    stk->sp     = stk->ptr + stk->sz;
  }

  (*stk->sp) = val;
  stk->sp++;
  stk->sz++;
}

Value* pop_stk(Stack* stk) {
  stk->sp--;
  stk->sz--;
  return stk->sp;
}

Value* peek_stk(Stack* stk, size_t dist) {
  return stk->sp - dist;
}

void reset_stk(Stack* stk) {
  stk->sz  = 0;
  stk->cap = SILKVM_STKSZ;
  stk->ptr = REALLOC_ARRAY(stk->ptr, Value, stk->cap, SILKVM_STKSZ);
  stk->sp  = stk->ptr;
}

void free_stk(Stack* stk) {
  FREE_ARRAY(stk->ptr, Value, stk->cap);
}
