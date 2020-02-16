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

#define RODATA(i) (vm->prg->rod.vls[i])

#define JUMP(z) (vm->ip += z)

#define SETERR(e) _->st = e;

#define ERROR(e)                                                               \
  do {                                                                         \
    vm->st = e;                                                                \
    return;                                                                    \
  } while (false)

#define FINISH() ERROR(STATUS_OK)

#define OFST() (uint32_t)(vm->ip - vm->prg->ins)
#define RET()  (vm->ip = vm->prg->ins + return_stk(&vm->stk))

#define TOP()     (top_stk(&vm->stk))
#define POP()     (pop_stk(&vm->stk))
#define PUSH(val) (push_stk(&vm->stk, val))

#define GET_LOCAL(i)    (get_stk_local(&vm->stk, i))
#define SET_LOCAL(i, v) (set_stk_local(&vm->stk, i, v))

#define TRUTHY() (truthy(TOP()))
#define FALSY()  (falsy(TOP()))

#define BOP(fct) PUSH(binary_op(vm, fct))
#define UOP(fct) PUSH(unary_op(vm, fct))

#define FUNC(f, ...) f(vm, ##__VA_ARGS__)

#define DEFINE_SYMBOL(i) hash_map_set(&vm->env, vm->prg->stb.syms[i], POP());

#define LOAD_SYMBOL(i)                                                         \
  do {                                                                         \
    Entry* entry = hash_map_get(&vm->env, vm->prg->stb.syms[i]);               \
    if (!entry) {                                                              \
      ERROR(STATUS_UNDEFN);                                                    \
    } else {                                                                   \
      PUSH(entry->value);                                                      \
    }                                                                          \
  } while (false)

#define ASSIGN_SYMBOL(i)                                                       \
  do {                                                                         \
    if (!hash_map_set_existing(&vm->env, vm->prg->stb.syms[i], TOP())) {       \
      ERROR(STATUS_UNDEFN);                                                    \
    }                                                                          \
  } while (false)

// ============================================================================
// === UTILITY ================================================================
// ============================================================================

#define TUP(a, b) a* b

#define NOTHING(...)

#ifdef __WIN32
  #define IS_BIG_ENDIAN 0
  #define SWAP_BYTES    _byteswap_uint64
#else
  #include <byteswap.h>
  #define IS_BIG_ENDIAN __BYTE_ORDER == __BIG_ENDIAN
  #define SWAP_BYTES    bswap_64
#endif

#define VAL_GET_STR(a)                                                         \
  IS_STR(a) ? a.as.string : ((ObjectString*)a.as.object)->data

#define CASE(C, A)                                                             \
  case C:                                                                      \
    A;                                                                         \
    break;

#ifndef NDEBUG
  #define PRINT_STRACE                                                         \
    printf("[");                                                               \
    for (Value* slt = frame_stk(&vm->stk)->bp; slt < vm->stk.vtop; slt++) {    \
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