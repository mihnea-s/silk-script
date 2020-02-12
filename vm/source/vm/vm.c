#include <stdbool.h>
#include <stdio.h>

#include <mem.h>
#include <opcode.h>
#include <stack.h>
#include <value.h>
#include <vm.h>

#define NEXT *(vm->ip++)

#define CNST vm->cnk->constants.vals[NEXT]

#define PUSH(val) push_stk(&vm->stk, val)
#define POP()     pop_stk(&vm->stk)

#define UNARY(op)                                                              \
  do {                                                                         \
    Value res = {                                                              \
      .type       = T_REAL,                                                    \
      .as.integer = op POP().as.integer,                                       \
    };                                                                         \
    PUSH(res);                                                                 \
  } while (false)

#define BINRY(op)                                                              \
  do {                                                                         \
    Value b   = POP();                                                         \
    Value a   = POP();                                                         \
    Value res = {                                                              \
      .type    = T_REAL,                                                       \
      .as.real = a.as.real op b.as.real,                                       \
    };                                                                         \
    PUSH(res);                                                                 \
  } while (false)

#define CASE(C, A)                                                             \
  case C: A; break;

void init_vm(VM* vm) {
  vm->cnk = NULL;
  vm->ip  = NULL;
  vm->st  = VM_OK;

  vm->stk = (Stack) {};
  init_stk(&vm->stk);
}

Value* peek(VM* vm, unsigned int distance) {
  return vm->stk.sp - distance;
}

void run(VM* vm, Chunk* cnk) {
  vm->cnk = cnk;
  vm->ip  = cnk->codes;

  for (;;) {
#define SILKVM_STRACE
#ifdef SILKVM_STRACE
    printf("[");
    for (Value* slt = vm->stk.ptr; slt < vm->stk.sp; slt++) {
      print_value(*slt);
      if (!(slt == vm->stk.sp - 1)) printf(", ");
    }
    printf("]\n");
#endif

    uint8_t ins = NEXT;
    switch (ins) {
      CASE(RET, return );
      CASE(VAL, PUSH(CNST));
      CASE(NEG, UNARY(-));
      CASE(ADD, BINRY(+));
      CASE(SUB, BINRY(-));
      CASE(MUL, BINRY(*));
      CASE(DIV, BINRY(/));
    }
  }
}

void free_vm(VM* vm) {
  free_stk(&vm->stk);
}