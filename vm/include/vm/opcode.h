#ifndef SILKVM_OPCODE_H
#define SILKVM_OPCODE_H

#include <stddef.h>

typedef enum {
  RET,
  VAL,
  NEG,
  ADD,
  SUB,
  DIV,
  MUL,
} OpCode;

#endif