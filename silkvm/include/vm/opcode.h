#ifndef SILKVM_OPCODE_H
#define SILKVM_OPCODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef enum {
  VM_RET, // return
  VM_VAL, // load value

  VM_NEG, // negation
  VM_NOT, // logical not

  VM_ADD, // addition
  VM_SUB, // subtraction
  VM_DIV, // float division
  VM_MUL, // multiplication
  VM_RIV, // round division
  VM_POW, // power
  VM_MOD, // modulo

  VM_NOP, // no operation
  VM_VID, // vid
  VM_TRU, // true
  VM_FAL, // false

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