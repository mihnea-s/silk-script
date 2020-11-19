#ifndef MOTHVM_PROGRAM_H
#define MOTHVM_PROGRAM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <moth/rodata.h>
#include <moth/symtable.h>

typedef struct {
  uint32_t cap;
  uint32_t len;
  Rodata   rod;
  Symtable stb;
  uint8_t* bytes;
} Program;

void     init_program(Program*, uint32_t, uint32_t, uint32_t);
void     write_byte(Program*, uint8_t);
uint32_t write_rodata(Program*, Value);
uint32_t write_symtable(Program*, Symbol);
void     free_program(Program*);

#ifdef __cplusplus
}
#endif

#endif