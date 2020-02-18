#ifndef MOTHVM_OPCODE_H
#define MOTHVM_OPCODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef enum {
  VM_FIN, // program finished

  VM_FRM,  // new call frame (1 byte)
  VM_FRM2, // 2 bytes
  VM_FRM3, // 3 bytes
  VM_FRM4, // 4 bytes

  VM_CLO, // close over
  VM_CAL, // function call
  VM_RET, // return

  VM_POP, // pop
  VM_PSH, // push value from stack to top (2 bytes)
  VM_STR, // store top value to index (2 bytes)

  VM_JMP, // unconditional jump (2 bytes)
  VM_JPT, // jump if true (2 bytes)
  VM_JPF, // jump if false (2 bytes)
  VM_JBW, // jump backwards (2 bytes)

  VM_VAL,  // load value (byte address)
  VM_VAL2, // 2 byte address
  VM_VAL3, // 3 byte address
  VM_VAL4, // 4 byte address

  VM_DEF,  // define symbol (byte address)
  VM_DEF2, // 2 byte address
  VM_DEF3, // 3 byte address
  VM_DEF4, // 4 byte address

  VM_SYM,  // load symbol (byte address)
  VM_SYM2, // 2 byte address
  VM_SYM3, // 3 byte address
  VM_SYM4, // 4 byte address

  VM_ASN,  // assign to symbol (byte address)
  VM_ASN2, // 2 byte address
  VM_ASN3, // 3 byte address
  VM_ASN4, // 4 byte address

  VM_NEG, // negation
  VM_NOT, // logical not
  VM_TYP, // type

  VM_ADD, // addition
  VM_SUB, // subtraction
  VM_DIV, // float division
  VM_MUL, // multiplication
  VM_RIV, // round division
  VM_POW, // power
  VM_MOD, // modulo

  // integer arithmetic
  VM_IADD,
  VM_ISUB,
  VM_IDIV,
  VM_IMUL,
  VM_IPOW,
  VM_IMOD,

  // real arithmetic
  VM_RADD,
  VM_RSUB,
  VM_RDIV,
  VM_RMUL,
  VM_RRIV,
  VM_RPOW,

  // strings
  VM_SCC, // string concat
  VM_SML, // string multiply

  VM_NOP, // no operation
  VM_VID, // vid
  VM_TRU, // true
  VM_FAL, // false

  VM_IS,  // is
  VM_ISN, // isnt

  VM_EQ,  // equal
  VM_NEQ, // not equal
  VM_GT,  // greater than
  VM_LT,  // less than
  VM_GTE, // greater than equal
  VM_LTE, // less than equal

  VM_PI,  // pi constant 3.141...
  VM_TAU, // tau constant 6.28...
  VM_EUL, // euler constant 2.78...

} OpCode;

#ifdef __cplusplus
}
#endif

#endif