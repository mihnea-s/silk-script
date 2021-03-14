#ifndef MOTHVM_MACROS_H
#define MOTHVM_MACROS_H

#ifdef __cplusplus
extern "C" {
#endif

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

#define SETERR(ERROR_CODE) vm->st = ERROR_CODE;

#define ERROR(ERROR_CODE)                                                      \
  do {                                                                         \
    vm->st = ERROR_CODE;                                                       \
    return;                                                                    \
  } while (false)

#define FINISH() return

#define BREAKPOINT()                                                           \
  do {                                                                         \
    vm->st = STATUS_BRKPNT;                                                    \
    return;                                                                    \
  } while (false)

// #define OFST() (uint32_t)(vm->ip - vm->prg->ins)
// #define RET()  (vm->ip = vm->prg->ins + stk_return(&vm->stk))

#define TOP()       (stk_top(&vm->stk))
#define POP()       (stk_pop(&vm->stk))
#define PUSH(VALUE) (stk_push(&vm->stk, VALUE))

#define GET_LOCAL(INDEX)        (stk_get(&vm->stk, INDEX))
#define SET_LOCAL(INDEX, VALUE) (stk_set(&vm->stk, INDEX, VALUE))

#define TRUTHY() (truthy(TOP()))
#define FALSY()  (falsy(TOP()))

#define BOP(FUNCTION) PUSH(binary_op(vm, FUNCTION))
#define UOP(FUNCTION) PUSH(unary_op(vm, FUNCTION))

#define FUNC(FUNCTION, ...) FUNCTION(vm, ##__VA_ARGS__)

#define DEFINE_SYMBOL(INDEX) env_set(&vm->env, vm->prg->stb.arr[INDEX], POP());

#define LOAD_SYMBOL(INDEX)                                                     \
  do {                                                                         \
    Entry *entry = env_get(&vm->env, vm->prg->stb.arr[INDEX]);                 \
    if (!entry) {                                                              \
      ERROR(STATUS_UNDEFN);                                                    \
    } else {                                                                   \
      PUSH(entry->value);                                                      \
    }                                                                          \
  } while (false)

#define ASSIGN_SYMBOL(INDEX)                                                   \
  do {                                                                         \
    if (!env_set_existing(&vm->env, vm->prg->stb.arr[INDEX], TOP())) {         \
      ERROR(STATUS_UNDEFN);                                                    \
    }                                                                          \
  } while (false)

// ============================================================================
// === UTILITY ================================================================
// ============================================================================

#define TUP(A, B) ((A * 1223) ^ B)

#define NOTHING(...)

#ifdef __WIN32
  #define IS_BIG_ENDIAN  0
  #define SWAP_BYTES     _byteswap_uint64
  #define PATH_SEPARATOR '\\'
#else
  #include <byteswap.h>
  #define IS_BIG_ENDIAN  __BYTE_ORDER == __BIG_ENDIAN
  #define SWAP_BYTES     bswap_64
  #define PATH_SEPARATOR '/'
#endif

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define SWAP(T, a, b)                                                          \
  do {                                                                         \
    T tmp = a;                                                                 \
    a     = b;                                                                 \
    b     = tmp;                                                               \
  } while (false)

#define VAL_GET_STR(VALUE)                                                     \
  IS_STR(VALUE) ? VALUE.as.string : ((ObjectString *)VALUE.as.object)->data

#define CASE(C, A)                                                             \
  case C:                                                                      \
    A;                                                                         \
    break;

#ifndef NDEBUG
  #define PRINT_STRACE                                                         \
    printf("[");                                                               \
    for (Value *slt = stk_frame(&vm->stk)->bp; slt < vm->stk.vtop; slt++) {    \
      print_value(*slt);                                                       \
      printf(", ");                                                            \
    }                                                                          \
    printf("]\n");
#else
  #define PRINT_STRACE
#endif

#ifdef __cplusplus
}
#endif

#endif