#ifndef SSRI_MACROS_H
#define SSRI_MACROS_H

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// === VM SETTINGS  ===========================================================
// ============================================================================

#define VEC_DIMEN_BYTES 8

// ============================================================================
// === VM EXECUTION ===========================================================
// ============================================================================

#define NEXT *(vm->ip++)

#define ARG1 (NEXT)
#define ARG2 ((ARG1 << 8) | NEXT)
#define ARG3 ((ARG2 << 8) | NEXT)
#define ARG4 ((ARG3 << 8) | NEXT)

#define RODATA(INDEX) (vm->prg->rod.arr[INDEX])

#define JUMP(OFFSET) (vm->ip += OFFSET)

#define SETERR(ERROR_CODE) vm->sts = ERROR_CODE;

#define ERROR(ERROR_CODE)                                                                          \
  do {                                                                                             \
    vm->sts = ERROR_CODE;                                                                          \
    return;                                                                                        \
  } while (false)

#define FINISH() return

#define BREAKPOINT()                                                                               \
  if (vm->dbg) {                                                                                   \
    vm->sts = STATUS_DEBUGD;                                                                       \
    return;                                                                                        \
  }

// #define OFST() (uint32_t)(vm->ip - vm->prg->ins)
// #define RET()  (vm->ip = vm->prg->ins + stk_return(&vm->stk))

#define TOP()       (stk_top(&vm->stk))
#define POP()       (stk_pop(&vm->stk))
#define PUSH(VALUE) (stk_push(&vm->stk, VALUE))

#define GET_LOCAL(INDEX)        (stk_get(&vm->stk, INDEX))
#define SET_LOCAL(INDEX, VALUE) (stk_set(&vm->stk, INDEX, VALUE))

#define TRUTHY() (truthy(POP()))
#define FALSY()  (falsy(POP()))

#define BOP(FUNCTION) PUSH(binary_op(vm, FUNCTION))
#define UOP(FUNCTION) PUSH(unary_op(vm, FUNCTION))

#define FUNC(FUNCTION, ...) FUNCTION(vm, ##__VA_ARGS__)

#define DEFINE_SYMBOL(INDEX) env_set(&vm->env, vm->prg->stb.arr[INDEX], POP());

#define LOAD_SYMBOL(INDEX)                                                                         \
  do {                                                                                             \
    Entry* entry = env_get(&vm->env, vm->prg->stb.arr[INDEX]);                                     \
    if (!entry) {                                                                                  \
      ERROR(STATUS_UNDEFN);                                                                        \
    } else {                                                                                       \
      PUSH(entry->value);                                                                          \
    }                                                                                              \
  } while (false)

#define ASSIGN_SYMBOL(INDEX)                                                                       \
  do {                                                                                             \
    if (!env_set_existing(&vm->env, vm->prg->stb.arr[INDEX], TOP())) { ERROR(STATUS_UNDEFN); }     \
  } while (false)

// ============================================================================
// === UTILITY ================================================================
// ============================================================================

#define TUP(A, B) ((A * 1223) ^ B)

#define IS_TYPE(A, B) (A.type == B)

#define NOTHING(...)

#define BYTE_VAL(x)                                                                                \
  (Value) {                                                                                        \
    .type = T_BYTE, .as.byte = x                                                                   \
  }
#define BOOL_VAL(x)                                                                                \
  (Value) {                                                                                        \
    .type = T_BOOL, .as.boolean = x                                                                \
  }
#define NATURAL_VAL(x)                                                                             \
  (Value) {                                                                                        \
    .type = T_NATURAL, .as.natural = x                                                             \
  }
#define INTEGER_VAL(x)                                                                             \
  (Value) {                                                                                        \
    .type = T_INTEGER, .as.integer = x                                                             \
  }
#define REAL_VAL(x)                                                                                \
  (Value) {                                                                                        \
    .type = T_REAL, .as.real = x                                                                   \
  }
#define VECTOR_VAL(x)                                                                              \
  (Value) {                                                                                        \
    .type = T_VECTOR, .as.vector = x                                                               \
  }
#define RUNE_VAL(x)                                                                                \
  (Value) {                                                                                        \
    .type = T_RUNE, .as.rune = x                                                                   \
  }
#define STRING_VAL(x)                                                                              \
  (Value) {                                                                                        \
    .type = T_STRING, .as.string = x                                                               \
  }
#define OBJECT_VAL(x)                                                                              \
  (Value) {                                                                                        \
    .type = T_OBJECT, .as.object = x                                                               \
  }
#define NATIVE_VAL(x)                                                                              \
  (Value) {                                                                                        \
    .type = T_NATIVE, .as.native = x                                                               \
  }

#define IS_OBJ_ERR(val) (IS_OBJ(val) && val.as.object->type == O_ERROR)
#define IS_OBJ_VEC(val) (IS_OBJ(val) && val.as.object->type == O_VECTOR)
#define IS_OBJ_STR(val) (IS_OBJ(val) && val.as.object->type == O_STRING)
#define IS_OBJ_DAT(val) (IS_OBJ(val) && val.as.object->type == O_DATA)
#define IS_OBJ_SET(val) (IS_OBJ(val) && val.as.object->type == O_SET)
#define IS_OBJ_DCT(val) (IS_OBJ(val) && val.as.object->type == O_DICTIONARY)
#define IS_OBJ_FCT(val) (IS_OBJ(val) && val.as.object->type == O_FUNCTION)
#define IS_OBJ_CLJ(val) (IS_OBJ(val) && val.as.object->type == O_CLOSURE)
#define IS_OBJ_HPV(val) (IS_OBJ(val) && val.as.object->type == O_HEAPVAL)

#define OBJ_ERR(obj) ((ObjectError*)obj)
#define OBJ_VEC(obj) ((ObjectVector*)obj)
#define OBJ_STR(obj) ((ObjectString*)obj)
#define OBJ_DAT(obj) ((ObjectData*)obj)
#define OBJ_SET(obj) ((ObjectSet*)obj)
#define OBJ_DCT(obj) ((ObjectDictionary*)obj)
#define OBJ_FCT(obj) ((ObjectFunction*)obj)
#define OBJ_CLJ(obj) ((ObjectClosure*)obj)
#define OBJ_HPV(obj) ((ObjectHeapval*)obj)

#ifdef __WIN32
  #define IS_BIG_ENDIAN  0
  #define SWAP_BYTES     _byteswap_uint64
  #define PATH_SEPARATOR '\\'
#else
  #include <byteswap.h>
  #define IS_BIG_ENDIAN  (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  #define SWAP_BYTES     bswap_64
  #define PATH_SEPARATOR '/'
#endif

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define SWAP(T, a, b)                                                                              \
  do {                                                                                             \
    T tmp = a;                                                                                     \
    a     = b;                                                                                     \
    b     = tmp;                                                                                   \
  } while (false)

#define CASE(C, A)                                                                                 \
  case C:                                                                                          \
    A;                                                                                             \
    break;

#if __has_attribute(__fallthrough__)
  #define FALLTHROUGH __attribute__((__fallthrough__))
#else
  #define FALLTHROUGH                                                                              \
    do {                                                                                           \
    } while (0) /* fallthrough */
#endif

#ifdef __cplusplus
}
#endif

#endif
