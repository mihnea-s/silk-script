#include <moth/vm.h>

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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

    // Value + Object
    // Swap then fallthough to next case
    case TUP(T_VOID, T_OBJ): // fallthrough
    case TUP(T_BOOL, T_OBJ): // fallthrough
    case TUP(T_CHAR, T_OBJ): // fallthrough
    case TUP(T_INT, T_OBJ):  // fallthrough
    case TUP(T_REAL, T_OBJ): // fallthrough
    case TUP(T_STR, T_OBJ): {
      SWAP(Value, a, b);
    }

    // Object + Value
    case TUP(T_OBJ, T_VOID): // fallthrough
    case TUP(T_OBJ, T_BOOL): // fallthrough
    case TUP(T_OBJ, T_CHAR): // fallthrough
    case TUP(T_OBJ, T_INT):  // fallthrough
    case TUP(T_OBJ, T_REAL): // fallthrough
    case TUP(T_OBJ, T_STR): {
      if (IS_OBJ_STR(a) && IS_STR(b)) {
        Object *obj =
          (Object *)obj_str_concat(OBJ_STR(a.as.object)->data, b.as.string);

        gc_register(&vm->gc, obj);
        return OBJ_VAL(obj);
      }

      if (IS_OBJ_ARR(a)) {
        Object *obj = (Object *)obj_arr_append(OBJ_ARR(a.as.object), b);
        gc_register(&vm->gc, obj);
        return OBJ_VAL(obj);
      }

      SETERR(STATUS_INVTYP);
      return VOID_VAL;
    }

    // Object + Object
    case TUP(T_OBJ, T_OBJ): {
      if (IS_OBJ_STR(a) && IS_OBJ_STR(b)) {
        Object *obj = (Object *)obj_str_concat(
          OBJ_STR(a.as.object)->data, OBJ_STR(b.as.object)->data);

        gc_register(&vm->gc, obj);
        return OBJ_VAL(obj);
      }

      // Other Object Types
      SETERR(STATUS_INVTYP);
      return VOID_VAL;
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
      Object *obj =
        (Object *)obj_str_concat_sep(a.as.string, PATH_SEPARATOR, b.as.string);

      gc_register(&vm->gc, obj);
      return OBJ_VAL(obj);
    }

    case TUP(T_STR, T_OBJ): {
      SWAP(Value, a, b);
      // fallthrough;
    }

    case TUP(T_OBJ, T_STR): {
      if (!IS_OBJ_STR(a)) {
        SETERR(STATUS_INVTYP);
        return VOID_VAL;
      }

      Object *obj = (Object *)obj_str_concat_sep(
        OBJ_STR(a.as.object)->data, PATH_SEPARATOR, b.as.string);

      gc_register(&vm->gc, obj);
      return OBJ_VAL(obj);
    }

    case TUP(T_OBJ, T_OBJ): {
      if (!IS_OBJ_STR(a) || !IS_OBJ_STR(b)) {
        SETERR(STATUS_INVTYP);
        return VOID_VAL;
      }

      Object *obj = (Object *)obj_str_concat_sep(
        OBJ_STR(a.as.object)->data, PATH_SEPARATOR, OBJ_STR(b.as.object)->data);

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

static inline Value index_(VM *vm, Value container, Value index) {
  bool is_string = (IS_OBJ_STR(container) || container.type == T_STR);

  // Handle strings (constant & heap alloc'd)
  if (is_string && index.type == T_INT) {
    char * str;
    size_t len;

    if (IS_OBJ_STR(container)) {
      str = OBJ_STR(container.as.object)->data;
      len = OBJ_STR(container.as.object)->size;
    } else {
      str = container.as.string;
      len = strlen(container.as.string);
    }

    if (index.as.integer >= len) {
      SETERR(STATUS_INVIDX);
      return VOID_VAL;
    }

    return CHAR_VAL(str[index.as.integer]);
  }

  // Handle vectors
  if (IS_OBJ_VEC(container) && index.type == T_INT) {
    if (index.as.integer >= OBJ_VEC(container.as.object)->card) {
      SETERR(STATUS_INVIDX);
      return VOID_VAL;
    }

    return REAL_VAL(OBJ_VEC(container.as.object)->comp[index.as.integer]);
  }

  // Arrays
  if (IS_OBJ_ARR(container) && index.type == T_INT) {
    if (index.as.integer >= OBJ_ARR(container.as.object)->size) {
      SETERR(STATUS_INVIDX);
      return VOID_VAL;
    }

    return OBJ_ARR(container.as.object)->vals[index.as.integer];
  }

  // Handle dictonaries
  if (IS_OBJ_DCT(container)) {
    if (!obj_dct_has_key(OBJ_DCT(container.as.object), index)) {
      SETERR(STATUS_INVIDX);
      return VOID_VAL;
    }

    return obj_dct_get(OBJ_DCT(container.as.object), index);
  }

  SETERR(STATUS_INVTYP);
  return VOID_VAL;
}

static inline Value indexasn_(VM *vm, Value index, Value value) {
  Value container = POP();

  if (!IS_OBJ(container)) {
    SETERR(STATUS_INVTYP);
    return VOID_VAL;
  }

  switch (container.as.object->type) {
    case O_ARRAY: {
      if (!IS_INT(index)) {
        SETERR(STATUS_INVTYP);
        return VOID_VAL;
      }

      OBJ_ARR(container.as.object)->vals[index.as.integer] = value;
      break;
    }

    case O_DICTIONARY: {
      obj_dct_insert(OBJ_DCT(container.as.object), index, value);
      break;
    }

    default: SETERR(STATUS_INVTYP); return VOID_VAL;
  }

  return container;
}

static inline Value merge_(VM *vm, Value a, Value b) {
  // Handle arrays
  if (IS_OBJ_ARR(a) && IS_OBJ_ARR(b)) {
    Object *obj =
      (Object *)obj_arr_concat(OBJ_ARR(a.as.object), OBJ_ARR(b.as.object));

    gc_register(&vm->gc, obj);
    return OBJ_VAL(obj);
  }

  // Handle dictonaries
  if (IS_OBJ_DCT(a) && IS_OBJ_DCT(b)) {
    obj_dct_merge(OBJ_DCT(a.as.object), OBJ_DCT(b.as.object));
    return a;
  }

  SETERR(STATUS_INVTYP);
  return VOID_VAL;
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
  return BOOL_VAL(equal_values(a, b));
}

static inline Value not_equal_(VM *vm, Value a, Value b) {
  return BOOL_VAL(!equal_values(a, b));
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
    case TUP(T_INT, T_INT): return BOOL_VAL(a.as.integer op b.as.integer);     \
    case TUP(T_REAL, T_REAL): return BOOL_VAL(a.as.real op b.as.real);         \
    case TUP(T_INT, T_REAL): return BOOL_VAL(a.as.integer op b.as.real);       \
    case TUP(T_REAL, T_INT): return BOOL_VAL(a.as.real op b.as.integer);       \
    case TUP(T_CHAR, T_CHAR): return BOOL_VAL(a.as.charac op b.as.charac);     \
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

static inline void promote_(VM *vm) {
  Value   promoted = POP();
  Object *obj      = (Object *)obj_hpv_promote(promoted);
  gc_register(&vm->gc, obj);
  PUSH(OBJ_VAL(obj));
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

static inline void array_(VM *vm, uint8_t size) {
  Object *obj = (Object *)obj_arr_with_size(size);
  gc_register(&vm->gc, obj);
  PUSH(OBJ_VAL(obj));
}

static inline void dictionary_(VM *vm, uint8_t size) {
  Object *obj = (Object *)obj_dct_with_cap(size);
  gc_register(&vm->gc, obj);
  PUSH(OBJ_VAL(obj));
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

#define MOTH_PRINT_ON_EXEC
#ifdef MOTH_PRINT_ON_EXEC
    PRINT_STRACE
#endif

    switch (NEXT) {
      // VM conditioning insturctions
      CASE(VM_FIN, FINISH());
      CASE(VM_NOP, NOTHING());
      CASE(VM_GC, gc_collect(&vm->gc));
      CASE(VM_DBG, BREAKPOINT());

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
      CASE(VM_PRO, FUNC(promote_));
      CASE(VM_RET, FUNC(return_));

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

      // Function operations
      CASE(VM_FRM, FUNC(frame_, ARG1));
      CASE(VM_FRM2, FUNC(frame_, ARG2));
      CASE(VM_FRM3, FUNC(frame_, ARG3));
      CASE(VM_FRM4, FUNC(frame_, ARG4));

      // Key values
      CASE(VM_VID, PUSH(VOID_VAL));
      CASE(VM_TRU, PUSH(BOOL_VAL(true)));
      CASE(VM_FAL, PUSH(BOOL_VAL(false)));

      // Mathematical constants
      CASE(VM_PI, PUSH(REAL_VAL(M_PI)));
      CASE(VM_TAU, PUSH(REAL_VAL(2.0 * M_PI)));
      CASE(VM_EUL, PUSH(REAL_VAL(M_E)));

      // Create operations
      CASE(VM_VEC, FUNC(vector_, ARG1));
      CASE(VM_ARR, FUNC(array_, ARG1));
      CASE(VM_DCT, FUNC(dictionary_, ARG1));

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

      // Indexing operations
      CASE(VM_IDX, BOP(index_));
      CASE(VM_IDA, BOP(indexasn_));
      CASE(VM_MRG, BOP(merge_));

      // Binary operations (boolean)
      CASE(VM_EQ, BOP(equal_));
      CASE(VM_NEQ, BOP(not_equal_));
      CASE(VM_GT, BOP(greater_));
      CASE(VM_LT, BOP(greater_eq_));
      CASE(VM_GTE, BOP(less_));
      CASE(VM_LTE, BOP(less_eq_));
    }
  } while (true);
}

void free_vm(VM *vm) {
  free_gc(&vm->gc);
  free_env(&vm->env);
}
