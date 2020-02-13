#ifndef SILKVM_MACROS_H
#define SILKVM_MACROS_H

// ============================================================================
// === VM EXECUTION ===========================================================
// ============================================================================

#define NEXT *(vm->ip++)
#define ARG  NEXT
#define VARG NEXT // TODO temporary

#define RODATA(i) vm->cnk->rod.vals[i]

#define PUSH(val) push_stk(&vm->stk, val)
#define POP()     pop_stk(&vm->stk)

// ============================================================================
// === UNARY OPERATIONS =======================================================
// ============================================================================

#define BOOL_UNARY_OP(op)                                                      \
  do {                                                                         \
    Value res = BOOL_VAL(op POP().as.boolean);                                 \
    PUSH(res);                                                                 \
  } while (false);

#define INT_UNARY_OP(op)                                                       \
  do {                                                                         \
    Value res = INT_VAL(op POP().as.integer);                                  \
    PUSH(res);                                                                 \
  } while (false);

#define AUTO_UNARY_OP(op)                                                      \
  do {                                                                         \
    Value a = POP();                                                           \
    if (IS_BOOL(a)) {                                                          \
      PUSH(BOOL_VAL(op a.as.boolean));                                         \
    } else if (IS_INT(a)) {                                                    \
      PUSH(INT_VAL(op a.as.integer));                                          \
    } else {                                                                   \
      vm->st = STATUS_INVT;                                                    \
    }                                                                          \
  } while (false)

// ============================================================================
// === BINARY OPERATIONS ======================================================
// ============================================================================

#define INT_BINARY_OP(op)                                                      \
  do {                                                                         \
    Value b   = POP();                                                         \
    Value a   = POP();                                                         \
    Value res = INT_VAL(a.as.integer op b.as.integer);                         \
    PUSH(res);                                                                 \
  } while (false);

#define INT_BINARY_FN(fn)                                                      \
  do {                                                                         \
    Value b   = POP();                                                         \
    Value a   = POP();                                                         \
    Value res = INT_VAL(fn(a.as.integer, b.as.integer));                       \
    PUSH(res);                                                                 \
  } while (false);

#define REAL_BINARY_OP(op)                                                     \
  do {                                                                         \
    Value b   = POP();                                                         \
    Value a   = POP();                                                         \
    Value res = REAL_VAL(a.as.real op b.as.real);                              \
    PUSH(res);                                                                 \
  } while (false);

#define REAL_BINARY_FN(fn)                                                     \
  do {                                                                         \
    Value b   = POP();                                                         \
    Value a   = POP();                                                         \
    Value res = REAL_VAL(fn(a.as.real, b.as.real));                            \
    PUSH(res);                                                                 \
  } while (false);

#define NUM_BINARY_OP(op)                                                      \
  do {                                                                         \
    Value b = POP();                                                           \
    Value a = POP();                                                           \
    if (IS_INT(a) && IS_INT(b)) {                                              \
      Value res = INT_VAL(a.as.integer op b.as.integer);                       \
      PUSH(res);                                                               \
    } else if (IS_REAL(a) && IS_REAL(b)) {                                     \
      Value res = REAL_VAL(a.as.real op b.as.real);                            \
      PUSH(res);                                                               \
    } else {                                                                   \
      vm->st = STATUS_INVT;                                                    \
    }                                                                          \
  } while (false)

#define CMP_EQ()  1
#define CMP_NEQ() 1
#define CMP_GT()  1
#define CMP_GTE() 1
#define CMP_LT()  1
#define CMP_LTE() 1

// ============================================================================
// === UTILITY ================================================================
// ============================================================================

#define MUL_FLR(a, b) floor(a* b)

#define NOTHING(...)

#define VAL_GET_STR(a)                                                         \
  IS_STR(a) ? a.as.string : ((ObjectString*)a.as.object)->data

#define CASE(C, A)                                                             \
  case C:                                                                      \
    A;                                                                         \
    break;

#define PRINT_STRACE                                                           \
  printf("[");                                                                 \
  for (Value* slt = vm->stk.ptr; slt < vm->stk.sp; slt++) {                    \
    print_value(*slt);                                                         \
    if (!(slt == vm->stk.sp - 1)) printf(", ");                                \
  }                                                                            \
  printf("]\n");

#endif