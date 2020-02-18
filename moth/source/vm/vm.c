#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <env.h>
#include <macros.h>
#include <mem.h>
#include <object.h>
#include <opcode.h>
#include <program.h>
#include <stack.h>
#include <value.h>
#include <vm.h>

typedef Value (*unary_op_fct)(VM*, Value);
static Value unary_op(VM* vm, unary_op_fct f) {
  return f(vm, POP());
}

typedef Value (*binary_op_fct)(VM*, Value, Value);
static Value binary_op(VM* vm, binary_op_fct f) {
  Value b = POP();
  Value a = POP();
  return f(vm, a, b);
}

//                                                                 //
//                                                                 //
//   _   _ _ __   __ _ _ __ _   _                                  //
//  | | | | '_ \ / _` | '__| | | |                                 //
//  | |_| | | | | (_| | |  | |_| |                                 //
//   \__,_|_| |_|\__,_|_|   \__, |                                 //
//                           __/ |                                 //
//                          |___/                                  //

static Value negate_(VM* _, Value a) {
  switch (a.type) {
    case T_INT: return INT_VAL(-a.as.integer);
    case T_REAL: return REAL_VAL(-a.as.real);
    default: SETERR(STATUS_INVTYP); return VID_VAL;
  }
}

static Value not_(VM* _, Value a) {
  return BOOL_VAL(falsy(a));
}

//   _     _                                                       //
//  | |   (_)                                                      //
//  | |__  _ _ __   __ _ _ __ _   _                                //
//  | '_ \| | '_ \ / _` | '__| | | |                               //
//  | |_) | | | | | (_| | |  | |_| |                               //
//  |_.__/|_|_| |_|\__,_|_|   \__, |                               //
//                             __/ |                               //
//                            |___/                                //

static Value add_(VM* _, Value a, Value b) {
  switch (TUP(a.type, b.type)) {
    case TUP(T_INT, T_INT): return INT_VAL(a.as.integer + b.as.integer);
    case TUP(T_REAL, T_REAL): return REAL_VAL(a.as.real + b.as.real);

    case TUP(T_STR, T_STR): {
      return OBJ_VAL((Object*)obj_str_concat(a.as.string, b.as.string));
    }

    case TUP(T_OBJ, T_OBJ): {
      if (IS_OBJ_STR(a) && IS_OBJ_STR(b)) {
        return OBJ_VAL((Object*)obj_str_concat(
          OBJ_STR(a.as.object)->data, OBJ_STR(b.as.object)->data));
        break;
      }
    }

    default: SETERR(STATUS_INVTYP); return VID_VAL;
  }
}

static Value subtract_(VM* _, Value a, Value b) {
  switch (TUP(a.type, b.type)) {
    case TUP(T_INT, T_INT): return INT_VAL(a.as.integer - b.as.integer);
    case TUP(T_REAL, T_REAL): return REAL_VAL(a.as.real - b.as.real);
    default: SETERR(STATUS_INVTYP); return VID_VAL;
  }
}

static Value divide_(VM* _, Value a, Value b) {
  switch (TUP(a.type, b.type)) {
    case TUP(T_REAL, T_REAL): return REAL_VAL(a.as.real / b.as.real);

    case TUP(T_INT, T_INT): {
      return REAL_VAL((double)a.as.integer / (double)b.as.integer);
    }

    default: SETERR(STATUS_INVTYP); return VID_VAL;
  }
}

static Value multiply_(VM* _, Value a, Value b) {
  switch (TUP(a.type, b.type)) {
    case TUP(T_INT, T_INT): return INT_VAL(a.as.integer * b.as.integer);
    case TUP(T_REAL, T_REAL): return REAL_VAL(a.as.real * b.as.real);
    default: SETERR(STATUS_INVTYP); return VID_VAL;
  }
}

static Value modulo_(VM* _, Value a, Value b) {
  switch (TUP(a.type, b.type)) {
    case TUP(T_INT, T_INT): return INT_VAL(a.as.integer % b.as.integer);
    default: SETERR(STATUS_INVTYP); return VID_VAL;
  }
}

static Value rounddiv_(VM* _, Value a, Value b) {
  switch (TUP(a.type, b.type)) {
    case TUP(T_INT, T_INT): return INT_VAL(a.as.integer / b.as.integer);
    case TUP(T_REAL, T_REAL): return INT_VAL(a.as.real / b.as.real);
    default: SETERR(STATUS_INVTYP); return VID_VAL;
  }
}

static Value power_(VM* _, Value a, Value b) {
  switch (TUP(a.type, b.type)) {
    case TUP(T_INT, T_INT): return INT_VAL(pow(a.as.integer, b.as.integer));
    case TUP(T_REAL, T_REAL): return REAL_VAL(pow(a.as.real, b.as.real));
    default: SETERR(STATUS_INVTYP); return VID_VAL;
  }
}

//                          _   _                                  //
//                         | |_| |                                 //
//    ___  __ _ _   _  __ _| (_) |_ _   _                          //
//   / _ \/ _` | | | |/ _` | | | __| | | |                         //
//  |  __/ (_| | |_| | (_| | | | |_| |_| |                         //
//   \___|\__, |\__,_|\__,_|_|_|\__|\__, |                         //
//           | |                     __/ |                         //
//           |_|                    |___/                          //

static Value equal_(VM* _, Value a, Value b) {
  if (a.type != b.type) return BOOL_VAL(false);
  switch (a.type) {
    case T_INT: return BOOL_VAL(a.as.integer == b.as.integer);
    case T_REAL: return BOOL_VAL(a.as.real == b.as.real);
    case T_BOOL: return BOOL_VAL(a.as.boolean == b.as.boolean);
    case T_VID: return BOOL_VAL(true);
    case T_STR: return BOOL_VAL(a.as.string == b.as.string);

    case T_OBJ: {
      if (a.as.object->type != b.as.object->type) return BOOL_VAL(false);
      return BOOL_VAL(true);
    }
  }
}

static Value not_equal_(VM* _, Value a, Value b) {
  if (a.type == b.type) return BOOL_VAL(false);
  switch (a.type) {
    case T_INT: return BOOL_VAL(a.as.integer != b.as.integer);
    case T_REAL: return BOOL_VAL(a.as.real != b.as.real);
    case T_BOOL: return BOOL_VAL(a.as.boolean != b.as.boolean);
    case T_VID: return BOOL_VAL(false);
    case T_STR: return BOOL_VAL(a.as.string != b.as.string);

    case T_OBJ: {
      if (a.as.object->type == b.as.object->type) return BOOL_VAL(false);
      return BOOL_VAL(false);
    }
  }
}

//                 _           _                                   //
//                | |         (_)                                  //
//    ___  _ __ __| | ___ _ __ _ _ __   __ _                       //
//   / _ \| '__/ _` |/ _ \ '__| | '_ \ / _` |                      //
//  | (_) | | | (_| |  __/ |  | | | | | (_| |                      //
//   \___/|_|  \__,_|\___|_|  |_|_| |_|\__, |                      //
//                                      __/ |                      //
//                                     |___/                       //

#define ORDOP(op)                                                              \
  switch (TUP(a.type, b.type)) {                                               \
    case TUP(T_INT, T_INT): return INT_VAL(a.as.integer > b.as.integer);       \
    case TUP(T_REAL, T_REAL): return REAL_VAL(a.as.real > b.as.real);          \
    default: SETERR(STATUS_INVTYP); return VID_VAL;                            \
  }

static Value greater_(VM* _, Value a, Value b) {
  ORDOP(>);
}

static Value greater_eq_(VM* _, Value a, Value b) {
  ORDOP(>=);
}

static Value less_(VM* _, Value a, Value b) {
  ORDOP(<);
}

static Value less_eq_(VM* _, Value a, Value b) {
  ORDOP(<=);
}

#undef ORDOP

//   _                                                             //
//  | |                                                            //
//  | |_ _   _ _ __   ___  ___                                     //
//  | __| | | | '_ \ / _ \/ __|                                    //
//  | |_| |_| | |_) |  __/\__ \                                    //
//   \__|\__, | .__/ \___||___/                                    //
//        __/ | |                                                  //
//       |___/|_|                                                  //

static Value typeof_(VM* _, Value a) {
  return INT_VAL(a.type);
}

static Value is_(VM* _, Value a, Value b) {
  return BOOL_VAL(a.type == b.as.integer);
}

static Value isnt_(VM* _, Value a, Value b) {
  return BOOL_VAL(a.type != b.as.integer);
}

//    __                  _   _                                    //
//   / _|                | | (_)                                   //
//  | |_ _   _ _ __   ___| |_ _  ___  _ __  ___                    //
//  |  _| | | | '_ \ / __| __| |/ _ \| '_ \/ __|                   //
//  | | | |_| | | | | (__| |_| | (_) | | | \__ \                   //
//  |_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|___/                   //
//                                                                 //
//                                                                 //

static void frame_(VM* vm, uint32_t addr) {
  stk_invoke(&vm->stk, vm->ip, ARG1);
  vm->ip = vm->prg->bytes + addr;
}

static void call_(VM* vm, uint8_t argc) {
  Value value = POP();

  if (IS_OBJ_FCT(value)) {
    ObjectFunction* fct = (ObjectFunction*)value.as.object;
    stk_invoke(&vm->stk, vm->ip, argc);
    vm->ip = fct->bytes;
    return;
  }

  ERROR(STATUS_NOTFCT);
}

static void return_(VM* vm) {
  Value ret = POP();
  vm->ip    = stk_return(&vm->stk);
  PUSH(ret);
}

//                            _   _                                //
//                           | | (_)                               //
//    _____  _____  ___ _   _| |_ _  ___  _ __                     //
//   / _ \ \/ / _ \/ __| | | | __| |/ _ \| '_ \                    //
//  |  __/>  <  __/ (__| |_| | |_| | (_) | | | |                   //
//   \___/_/\_\___|\___|\__,_|\__|_|\___/|_| |_|                   //
//                                                                 //
//                                                                 //

void init_vm(VM* vm) {
  vm->prg = NULL;
  vm->ip  = NULL;
  vm->st  = STATUS_OK;

  // initialize VM stack
  init_stk(&vm->stk);

  // initialize global env
  init_env(&vm->env);
}

void run(VM* vm, Program* prog) {
  vm->prg = prog;
  vm->ip  = prog->bytes;
  vm->st  = STATUS_OK;

  do {
    PRINT_STRACE

    switch (NEXT) {
      CASE(VM_FIN, FINISH());

      // stack operations
      CASE(VM_POP, POP());
      CASE(VM_PSH, PUSH(GET_LOCAL(ARG2)));
      CASE(VM_STR, SET_LOCAL(ARG2, TOP()));

      // rodata operations
      CASE(VM_VAL, PUSH(RODATA(ARG1)));
      CASE(VM_VAL2, PUSH(RODATA(ARG2)));
      CASE(VM_VAL3, PUSH(RODATA(ARG3)));
      CASE(VM_VAL4, PUSH(RODATA(ARG4)));

      // jumps
      CASE(VM_JMP, JUMP(ARG2));
      CASE(VM_JPT, JUMP(TRUTHY() * ARG2));
      CASE(VM_JPF, JUMP(FALSY() * ARG2));
      CASE(VM_JBW, JUMP(-(int64_t)ARG2));

      // function operations
      CASE(VM_FRM, FUNC(frame_, ARG1));
      CASE(VM_FRM2, FUNC(frame_, ARG2));
      CASE(VM_FRM3, FUNC(frame_, ARG3));
      CASE(VM_FRM4, FUNC(frame_, ARG4));
      CASE(VM_CAL, FUNC(call_, ARG1));
      CASE(VM_RET, FUNC(return_));

      // unary operations
      CASE(VM_NEG, UOP(negate_));
      CASE(VM_NOT, UOP(not_));

      // arithmetic
      CASE(VM_ADD, BOP(add_));
      CASE(VM_SUB, BOP(subtract_));
      CASE(VM_MUL, BOP(multiply_));
      CASE(VM_DIV, BOP(divide_));
      CASE(VM_RIV, BOP(rounddiv_));
      CASE(VM_POW, BOP(power_));
      CASE(VM_MOD, BOP(modulo_));

      // boolean operations
      CASE(VM_EQ, BOP(equal_));
      CASE(VM_NEQ, BOP(not_equal_));
      CASE(VM_GT, BOP(greater_));
      CASE(VM_LT, BOP(greater_eq_));
      CASE(VM_GTE, BOP(less_));
      CASE(VM_LTE, BOP(less_eq_));

      // type operations
      CASE(VM_TYP, UOP(typeof_));
      CASE(VM_IS, BOP(is_));
      CASE(VM_ISN, BOP(isnt_));

      // constant values
      CASE(VM_NOP, NOTHING());
      CASE(VM_VID, PUSH(VID_VAL));
      CASE(VM_TRU, PUSH(BOOL_VAL(true)));
      CASE(VM_FAL, PUSH(BOOL_VAL(false)));

      // constants
      CASE(VM_PI, PUSH(REAL_VAL(M_PI)));
      CASE(VM_TAU, PUSH(REAL_VAL(2.0 * M_PI)));
      CASE(VM_EUL, PUSH(REAL_VAL(M_E)));

      // symbol operations
      CASE(VM_DEF, DEFINE_SYMBOL(ARG1));
      CASE(VM_DEF2, DEFINE_SYMBOL(ARG2));
      CASE(VM_DEF3, DEFINE_SYMBOL(ARG3));
      CASE(VM_DEF4, DEFINE_SYMBOL(ARG4));

      CASE(VM_SYM, LOAD_SYMBOL(ARG1));
      CASE(VM_SYM2, LOAD_SYMBOL(ARG2));
      CASE(VM_SYM3, LOAD_SYMBOL(ARG3));
      CASE(VM_SYM4, LOAD_SYMBOL(ARG4));

      CASE(VM_ASN, ASSIGN_SYMBOL(ARG1));
      CASE(VM_ASN2, ASSIGN_SYMBOL(ARG2));
      CASE(VM_ASN3, ASSIGN_SYMBOL(ARG3));
      CASE(VM_ASN4, ASSIGN_SYMBOL(ARG4));
    }
  } while (true);
}

void free_vm(VM* vm) {
  free_stk(&vm->stk);
  free_env(&vm->env);
}
