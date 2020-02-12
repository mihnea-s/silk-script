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

} OpCode;

#ifdef __cplusplus
}
#endif

#endif