#include <bits/stdint-uintn.h>
#include <moth/vm.h>

#include <math.h>
#include <stdio.h>

#include <moth/env.h>
#include <moth/garbage.h>
#include <moth/macros.h>
#include <moth/mem.h>
#include <moth/object.h>
#include <moth/opcode.h>
#include <moth/program.h>
#include <moth/stack.h>
#include <moth/value.h>

typedef Value (*unary_op_fct)(VM *, Value);
static inline Value unary_op(VM *vm, unary_op_fct f) {
  return f(vm, POP());
}

typedef Value (*binary_op_fct)(VM *, Value, Value);
static inline Value binary_op(VM *vm, binary_op_fct f) {
  Value b = POP();
  Value a = POP();
  return f(vm, a, b);
}

//   _   _ _ __   __ _ _ __ _   _                                  //
//  | | | | '_ \ / _` | '__| | | |                                 //
//  | |_| | | | | (_| | |  | |_| |                                 //
//   \__,_|_| |_|\__,_|_|   \__, |                                 //
//                           __/ |                                 //
//                          |___/                                  //

static inline Value negate_(VM *vm, Value a) {
  switch (a.type) {
    case T_INT: return INT_VAL(-a.as.integer);
    case T_REAL: return REAL_VAL(-a.as.real);
    default: SETERR(STATUS_INVTYP); return VOID_VAL;
  }
}

static inline Value not_(VM *vm, Value a) {
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

static inline Value add_(VM *vm, Value a, Value b) {
  switch (TUP(a.type, b.type)) {
    case TUP(T_INT, T_INT): return INT_VAL(a.as.integer + b.as.integer);
    case TUP(T_REAL, T_REAL): return REAL_VAL(a.as.real + b.as.real);

    case TUP(T_INT, T_REAL): return REAL_VAL(a.as.integer + b.as.real);
    case TUP(T_REAL, T_INT): return REAL_VAL(a.as.real + b.as.integer);

    case TUP(T_STR, T_STR): {
      Object *str = (Object *)obj_str_concat(a.as.string, b.as.string);
      gc_register(&vm->gc, str);
      return OBJ_VAL(str);
    }

    case TUP(T_OBJ, T_OBJ): {
      if (IS_OBJ_STR(a) && IS_OBJ_STR(b)) {
        Object *str = (Object *)obj_str_concat(
          OBJ_STR(a.as.object)->data, OBJ_STR(b.as.object)->data);
        gc_register(&vm->gc, str);
        return OBJ_VAL(str);
      }

      if (IS_OBJ_ARR(a)) {
        obj_arr_append(OBJ_ARR(a.as.object), b);
        return a;
      }
    }

    default: SETERR(STATUS_INVTYP); return VOID_VAL;
  }
}

static inline Value subtract_(VM *vm, Value a, Value b) {
  switch (TUP(a.type, b.type)) {
    case TUP(T_INT, T_INT): return INT_VAL(a.as.integer - b.as.integer);
    case TUP(T_REAL, T_REAL): return REAL_VAL(a.as.real - b.as.real);
    case TUP(T_REAL, T_INT): return REAL_VAL(a.as.real - b.as.integer);
    case TUP(T_INT, T_REAL): return REAL_VAL(a.as.integer - b.as.real);
    default: SETERR(STATUS_INVTYP); return VOID_VAL;
  }
}

static inline Value divide_(VM *vm, Value a, Value b) {
  switch (TUP(a.type, b.type)) {
    case TUP(T_REAL, T_REAL): return REAL_VAL(a.as.real / b.as.real);
    case TUP(T_INT, T_REAL): return REAL_VAL((double)a.as.integer * b.as.real);
    case TUP(T_REAL, T_INT): return REAL_VAL(a.as.real / (double)b.as.integer);

    case TUP(T_INT, T_INT): {
      return REAL_VAL((double)a.as.integer / (double)b.as.integer);
    }

    case TUP(T_STR, T_STR): {
      // TODO path separator
      Object *obj = (Object *)obj_str_concat(
        OBJ_STR(a.as.object)->data, OBJ_STR(b.as.object)->data);
      gc_register(&vm->gc, obj);
      return OBJ_VAL(obj);
    }

    default: SETERR(STATUS_INVTYP); return VOID_VAL;
  }
}

static inline Value multiply_(VM *vm, Value a, Value b) {
  switch (TUP(a.type, b.type)) {
    case TUP(T_INT, T_INT): return INT_VAL(a.as.integer * b.as.integer);
    case TUP(T_REAL, T_REAL): return REAL_VAL(a.as.real * b.as.real);
    case TUP(T_INT, T_REAL): return REAL_VAL(a.as.integer * b.as.real);
    case TUP(T_REAL, T_INT): return REAL_VAL(a.as.real * b.as.integer);
    default: SETERR(STATUS_INVTYP); return VOID_VAL;
  }
}

static inline Value modulo_(VM *vm, Value a, Value b) {
  switch (TUP(a.type, b.type)) {
    case TUP(T_INT, T_INT): return INT_VAL(a.as.integer % b.as.integer);
    case TUP(T_REAL, T_REAL): return REAL_VAL(fmod(a.as.real, b.as.real));
    case TUP(T_INT, T_REAL): return REAL_VAL(fmod(a.as.integer, b.as.real));
    case TUP(T_REAL, T_INT): return REAL_VAL(fmod(a.as.real, b.as.integer));
    default: SETERR(STATUS_INVTYP); return VOID_VAL;
  }
}

static inline Value rounddiv_(VM *vm, Value a, Value b) {
  switch (TUP(a.type, b.type)) {
    case TUP(T_INT, T_INT): return INT_VAL(a.as.integer / b.as.integer);
    case TUP(T_REAL, T_REAL): return INT_VAL(a.as.real / b.as.real);
    case TUP(T_INT, T_REAL): return INT_VAL(a.as.integer / b.as.real);
    case TUP(T_REAL, T_INT): return INT_VAL(a.as.real / b.as.integer);
    default: SETERR(STATUS_INVTYP); return VOID_VAL;
  }
}

static inline Value power_(VM *vm, Value a, Value b) {
  switch (TUP(a.type, b.type)) {
    case TUP(T_INT, T_INT): return INT_VAL(pow(a.as.integer, b.as.integer));
    case TUP(T_REAL, T_REAL): return REAL_VAL(pow(a.as.real, b.as.real));
    case TUP(T_INT, T_REAL): return REAL_VAL(pow(a.as.integer, b.as.real));
    case TUP(T_REAL, T_INT): return REAL_VAL(pow(a.as.real, b.as.integer));
    default: SETERR(STATUS_INVTYP); return VOID_VAL;
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

static inline Value equal_(VM *vm, Value a, Value b) {
  if (a.type != b.type) return BOOL_VAL(false);
  switch (a.type) {
    case T_VOID: return BOOL_VAL(true);
    case T_BOOL: return BOOL_VAL(a.as.boolean == b.as.boolean);
    case T_INT: return BOOL_VAL(a.as.integer == b.as.integer);
    case T_REAL: return BOOL_VAL(a.as.real == b.as.real);
    case T_CHAR: return BOOL_VAL(a.as.charac == b.as.charac);
    case T_STR: return BOOL_VAL(a.as.string == b.as.string);

    case T_OBJ: {
      if (a.as.object->type != b.as.object->type) return BOOL_VAL(false);
      return BOOL_VAL(true);
    }
  }
}

static inline Value not_equal_(VM *vm, Value a, Value b) {
  if (a.type == b.type) return BOOL_VAL(false);
  switch (a.type) {
    case T_VOID: return BOOL_VAL(false);
    case T_INT: return BOOL_VAL(a.as.integer != b.as.integer);
    case T_REAL: return BOOL_VAL(a.as.real != b.as.real);
    case T_BOOL: return BOOL_VAL(a.as.boolean != b.as.boolean);
    case T_CHAR: return BOOL_VAL(a.as.charac != b.as.charac);
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

#define NUM_ORDERING_OP(op)                                                    \
  switch (TUP(a.type, b.type)) {                                               \
    case TUP(T_INT, T_INT): return INT_VAL(a.as.integer op b.as.integer);      \
    case TUP(T_REAL, T_REAL): return REAL_VAL(a.as.real op b.as.real);         \
    case TUP(T_INT, T_REAL): return INT_VAL(a.as.integer op b.as.real);        \
    case TUP(T_REAL, T_INT): return REAL_VAL(a.as.real op b.as.integer);       \
    default: SETERR(STATUS_INVTYP); return VOID_VAL;                           \
  }

static inline Value greater_(VM *vm, Value a, Value b) {
  NUM_ORDERING_OP(>);
}

static inline Value greater_eq_(VM *vm, Value a, Value b) {
  NUM_ORDERING_OP(>=);
}

static inline Value less_(VM *vm, Value a, Value b) {
  NUM_ORDERING_OP(<);
}

static inline Value less_eq_(VM *vm, Value a, Value b) {
  NUM_ORDERING_OP(<=);
}

#undef NUM_ORDERING_OP

//    __                  _   _                                    //
//   / _|                | | (_)                                   //
//  | |_ _   _ _ __   ___| |_ _  ___  _ __  ___                    //
//  |  _| | | | '_ \ / __| __| |/ _ \| '_ \/ __|                   //
//  | | | |_| | | | | (__| |_| | (_) | | | \__ \                   //
//  |_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|___/                   //
//                                                                 //
//                                                                 //

static inline void frame_(VM *vm, uint32_t addr) {
  stk_invoke(&vm->stk, vm->ip, ARG1);
  vm->ip = vm->prg->bytes + addr;
}

static inline void closeover_(VM *vm) {
  Value val = POP();
  if (!IS_OBJ_FCT(val)) ERROR(STATUS_INVTYP);

  Object *closure = (Object *)obj_clj_from_fct(OBJ_FCT(val.as.object));
  gc_register(&vm->gc, closure);
  PUSH(OBJ_VAL(closure));
}

static inline void call_(VM *vm, uint8_t argc) {
  Value value = POP();

  if (IS_OBJ_FCT(value)) {
    ObjectFunction *fct = OBJ_FCT(value.as.object);
    stk_invoke(&vm->stk, vm->ip, argc);
    vm->ip = fct->bytes;
    return;
  }

  if (IS_OBJ_CLJ(value)) {
    ObjectClosure *clj = OBJ_CLJ(value.as.object);
    stk_invoke(&vm->stk, vm->ip, argc);
    vm->ip = clj->fct->bytes;
    return;
  }

  ERROR(STATUS_NOTFUN);
}

static inline void return_(VM *vm) {
  Value ret = POP();
  vm->ip    = stk_return(&vm->stk);
  PUSH(ret);
}

static inline void vector_(VM *vm, uint8_t card) {
  double *comps = memory(NULL, 0, sizeof(double) * card);

  for (uint8_t i = 0; i < card; i++) {
    Value value = POP();

    switch (value.type) {
      case T_INT: comps[i] = value.as.integer;
      case T_REAL: comps[i] = value.as.real;
      default:
        SETERR(STATUS_INVARG);
        release(comps, sizeof(double) * card);
        return;
    }
  }

  PUSH(OBJ_VAL((Object *)obj_vec_from_raw(comps, card)));
  release(comps, sizeof(double) * card);
}

static inline void array_(VM *vm, uint32_t size) {
}

//                            _   _                                //
//                           | | (_)                               //
//    _____  _____  ___ _   _| |_ _  ___  _ __                     //
//   / _ \ \/ / _ \/ __| | | | __| |/ _ \| '_ \                    //
//  |  __/>  <  __/ (__| |_| | |_| | (_) | | | |                   //
//   \___/_/\_\___|\___|\__,_|\__|_|\___/|_| |_|                   //
//                                                                 //
//                                                                 //

void init_vm(VM *vm) {
  vm->prg = NULL;
  vm->ip  = NULL;
  vm->st  = STATUS_OK;

  // initialize VM stack
  init_stk(&vm->stk);

  // initialize global env
  init_env(&vm->env);

  // initialize garbage collection
  init_gc(&vm->gc, &vm->stk);
}

void vm_run(VM *vm, Program *prog) {
  vm->prg = prog;
  vm->ip  = prog->bytes;
  vm->st  = STATUS_OK;

  do {
    PRINT_STRACE

    switch (NEXT) {
      // VM conditioning insturctions
      CASE(VM_FIN, FINISH());
      CASE(VM_NOP, NOTHING());
      CASE(VM_GC, gc_collect(&vm->gc));

      // Stack operations
      CASE(VM_POP, POP());
      CASE(VM_PSH, PUSH(GET_LOCAL(ARG2)));
      CASE(VM_STR, SET_LOCAL(ARG2, TOP()));

      // Jumps
      CASE(VM_JMP, JUMP(ARG2));
      CASE(VM_JPT, JUMP(TRUTHY() * ARG2));
      CASE(VM_JPF, JUMP(FALSY() * ARG2));
      CASE(VM_JBW, JUMP(-(int64_t)ARG2));

      // Function operations
      CASE(VM_CLO, FUNC(closeover_));
      CASE(VM_CAL, FUNC(call_, ARG1));
      CASE(VM_RET, FUNC(return_));
      CASE(VM_RETV, FUNC(return_));

      // Rodata operations
      CASE(VM_VAL, PUSH(RODATA(ARG1)));
      CASE(VM_VAL2, PUSH(RODATA(ARG2)));
      CASE(VM_VAL3, PUSH(RODATA(ARG3)));
      CASE(VM_VAL4, PUSH(RODATA(ARG4)));

      // Load a symbol to be defined / assigned to
      CASE(VM_SYM, LOAD_SYMBOL(ARG1));
      CASE(VM_SYM2, LOAD_SYMBOL(ARG2));
      CASE(VM_SYM3, LOAD_SYMBOL(ARG3));
      CASE(VM_SYM4, LOAD_SYMBOL(ARG4));

      // Define a symbol in the environment
      CASE(VM_DEF, DEFINE_SYMBOL(ARG1));
      CASE(VM_DEF2, DEFINE_SYMBOL(ARG2));
      CASE(VM_DEF3, DEFINE_SYMBOL(ARG3));
      CASE(VM_DEF4, DEFINE_SYMBOL(ARG4));

      // Assign to a symbol in the environment
      CASE(VM_ASN, ASSIGN_SYMBOL(ARG1));
      CASE(VM_ASN2, ASSIGN_SYMBOL(ARG2));
      CASE(VM_ASN3, ASSIGN_SYMBOL(ARG3));
      CASE(VM_ASN4, ASSIGN_SYMBOL(ARG4));

      // function operations
      CASE(VM_FRM, FUNC(frame_, ARG1));
      CASE(VM_FRM2, FUNC(frame_, ARG2));
      CASE(VM_FRM3, FUNC(frame_, ARG3));
      CASE(VM_FRM4, FUNC(frame_, ARG4));

      // Create vectors / arrays
      CASE(VM_VEC, FUNC(vector_, ARG1));
      CASE(VM_ARR, FUNC(array_, ARG1));
      CASE(VM_ARR2, FUNC(array_, ARG2));
      CASE(VM_ARR3, FUNC(array_, ARG3));
      CASE(VM_ARR4, FUNC(array_, ARG4));

      // Key values
      CASE(VM_VID, PUSH(VOID_VAL));
      CASE(VM_TRU, PUSH(BOOL_VAL(true)));
      CASE(VM_FAL, PUSH(BOOL_VAL(false)));

      // Mathematical constants
      CASE(VM_PI, PUSH(REAL_VAL(M_PI)));
      CASE(VM_TAU, PUSH(REAL_VAL(2.0 * M_PI)));
      CASE(VM_EUL, PUSH(REAL_VAL(M_E)));

      // Unary operations
      CASE(VM_NEG, UOP(negate_));
      CASE(VM_NOT, UOP(not_));

      // Binary operations (arithmetic)
      CASE(VM_ADD, BOP(add_));
      CASE(VM_SUB, BOP(subtract_));
      CASE(VM_MUL, BOP(multiply_));
      CASE(VM_DIV, BOP(divide_));
      CASE(VM_RIV, BOP(rounddiv_));
      CASE(VM_POW, BOP(power_));
      CASE(VM_MOD, BOP(modulo_));

      // Binary operations (boolean)
      CASE(VM_EQ, BOP(equal_));
      CASE(VM_NEQ, BOP(not_equal_));
      CASE(VM_GT, BOP(greater_));
      CASE(VM_LT, BOP(greater_eq_));
      CASE(VM_GTE, BOP(less_));
      CASE(VM_LTE, BOP(less_eq_));

      // Debugging instructions
      CASE(VM_DBG_LOC, );
      CASE(VM_DBG_BRK, );
    }
  } while (true);
}

void free_vm(VM *vm) {
  free_gc(&vm->gc);
  free_env(&vm->env);
}
