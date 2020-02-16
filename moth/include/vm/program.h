#ifndef MOTHVM_PROGRAM_H
#define MOTHVM_PROGRAM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "rodata.h"
#include "symtable.h"

typedef struct {
  uint32_t cap;
  uint32_t len;
  Rodata   rod;
  Symtable stb;
  uint8_t* ins;
} Program;

void     init_program(Program*, uint32_t, uint32_t, uint32_t);
void     write_ins(Program*, uint8_t);
uint32_t write_rod(Program*, Value);
uint32_t write_sym(Program*, Symbol);
void     free_program(Program*);

#ifdef __cplusplus
}
#endif

#endif