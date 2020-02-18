#include <stdint.h>

#include <mem.h>
#include <stack.h>
#include <value.h>

void init_stk(Stack* stk) {
  stk->ftop = stk->farr;
  stk->vtop = stk->varr;

  stk_invoke(stk, 0x0, 0x0);
}

Value stk_top(Stack* stk) {
  return *(stk->vtop - 1);
}

Value stk_pop(Stack* stk) {
  stk->vtop--;
  return *stk->vtop;
}

void stk_push(Stack* stk, Value val) {
  (*stk->vtop) = val;
  stk->vtop++;
}

Frame* stk_frame(Stack* stk) {
  return stk->ftop - 1;
}

uint8_t* stk_return(Stack* stk) {
  stk->ftop--;
  stk->vtop = stk->ftop->bp;
  return stk->ftop->ra;
}

void stk_invoke(Stack* stk, uint8_t* ra, uint8_t argc) {
  stk->ftop->ra = ra;
  stk->ftop->bp = stk->vtop - argc;
  stk->ftop++;
}

Value stk_get(Stack* stk, size_t i) {
  return *(stk_frame(stk)->bp + i);
}

void stk_set(Stack* stk, size_t i, Value val) {
  *(stk_frame(stk)->bp + i) = val;
}

void reset_stk(Stack* stk) {
  init_stk(stk);
}

void free_stk(Stack* stk) {
  // nothing do to this function
  // is just for consistency
}
