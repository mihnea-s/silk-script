#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include <chunk.h>
#include <macros.h>
#include <mem.h>
#include <opcode.h>
#include <program.h>
#include <stack.h>
#include <value.h>
#include <vm.h>

void init_vm(VM* vm) {
  vm->cnk = NULL;
  vm->ip  = NULL;
  vm->st  = STATUS_OK;

  vm->stk = (Stack) {};
  init_stk(&vm->stk);
}

void run(VM* vm, Program* prog) {
  Chunk* cnk = prog->cnks;

  vm->cnk = cnk;
  vm->ip  = cnk->codes;

  while (vm->st == STATUS_OK) {
    PRINT_STRACE

    switch (NEXT) {
      CASE(VM_RET, return );
      CASE(VM_VAL, PUSH(RODATA(ARG)));

      CASE(VM_NEG, INT_UNARY_OP(-));
      CASE(VM_NOT, PUSH(BOOL_VAL(falsy(POP()))));

      CASE(VM_SUB, NUM_BINARY_OP(-));
      CASE(VM_DIV, NUM_BINARY_OP(/));
      CASE(VM_RIV, REAL_BINARY_FN(MUL_FLR));
      CASE(VM_POW, REAL_BINARY_FN(pow));
      CASE(VM_MOD, INT_BINARY_OP(%));

      CASE(VM_NOP, );
      CASE(VM_VID, PUSH(VID_VAL));
      CASE(VM_TRU, PUSH(BOOL_VAL(true)));
      CASE(VM_FAL, PUSH(BOOL_VAL(false)));

      CASE(VM_EQ, CMP_EQ());
      CASE(VM_NEQ, CMP_NEQ());
      CASE(VM_GT, CMP_GT());
      CASE(VM_LT, CMP_LT());
      CASE(VM_GTE, CMP_GTE());
      CASE(VM_LTE, CMP_LTE());

      CASE(VM_PI, PUSH(REAL_VAL(M_PI)));
      CASE(VM_TAU, PUSH(REAL_VAL(2.0 * M_PI)));
      CASE(VM_EUL, PUSH(REAL_VAL(M_E)));

      case VM_ADD: {
        Value b = POP();
        Value a = POP();
        if (IS_INT(a) && IS_INT(b)) {
          Value res = INT_VAL(a.as.integer + b.as.integer);
          PUSH(res);
        } else if (IS_REAL(a) && IS_REAL(b)) {
          Value res = REAL_VAL(a.as.real + b.as.real);
          PUSH(res);
        } else if (
          (IS_STR(a) || IS_OBJ_STR(a)) && (IS_STR(b) || IS_OBJ_STR(b))) {
          PUSH(
            OBJ_VAL((Object*)obj_str_concat(VAL_GET_STR(a), VAL_GET_STR(b))));
        } else {
          vm->st = STATUS_INVT;
        }

        break;
      }

      case VM_MUL: {
        Value b = POP();
        Value a = POP();
        if (IS_INT(a) && IS_INT(b)) {
          Value res = INT_VAL(a.as.integer * b.as.integer);
          PUSH(res);
        } else if (IS_REAL(a) && IS_REAL(b)) {
          Value res = REAL_VAL(a.as.real * b.as.real);
          PUSH(res);
        } else if ((IS_STR(a) || IS_OBJ_STR(a)) && IS_INT(b)) {
          PUSH(
            OBJ_VAL((Object*)obj_str_multiply(VAL_GET_STR(a), b.as.integer)));
        } else {
          vm->st = STATUS_INVT;
        }

        break;
      }
    }
  }
}

void free_vm(VM* vm) {
  free_stk(&vm->stk);
}
