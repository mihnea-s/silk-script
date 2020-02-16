#include <stdint.h>

#include <mem.h>
#include <stack.h>
#include <value.h>

void init_stk(Stack* stk) {
  stk->itop = stk->iarr;
  stk->vtop = stk->varr;

  invoke_stk(stk, 0x0);
}

Value top_stk(Stack* stk) {
  return *(stk->vtop - 1);
}

Value pop_stk(Stack* stk) {
  stk->vtop--;
  return *stk->vtop;
}

void push_stk(Stack* stk, Value val) {
  (*stk->vtop) = val;
  stk->vtop++;
}

Invokation* frame_stk(Stack* stk) {
  return stk->itop - 1;
}

uint8_t* return_stk(Stack* stk) {
  stk->itop--;
  return stk->itop->rt;
}

void invoke_stk(Stack* stk, uint8_t* rt) {
  stk->itop->rt = rt;
  stk->itop->bp = stk->vtop;
  stk->itop++;
}

Value get_stk_local(Stack* stk, size_t i) {
  return *(frame_stk(stk)->bp + i);
}

void set_stk_local(Stack* stk, size_t i, Value val) {
  *(frame_stk(stk)->bp + i) = val;
}

void reset_stk(Stack* stk) {
  init_stk(stk);
}

void free_stk(Stack* stk) {
  // nothing do to this function
  // is just for consistency
}
