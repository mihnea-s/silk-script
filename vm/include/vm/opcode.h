#ifndef SILKVM_OPCODE_H
#define SILKVM_OPCODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef enum {
  RET, // return
  VAL, // load value

  NEG, // negation

  ADD, // addition
  SUB, // subtraction
  DIV, // float division
  MUL, // multiplication
  RIV, // round division
  POW, // power
  MOD, // modulo

  NOP, // no operation
  VID, // vid

} OpCode;

#ifdef __cplusplus
}
#endif

#endif