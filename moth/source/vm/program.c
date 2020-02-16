#include <stdint.h>

#include <mem.h>
#include <program.h>
#include <rodata.h>
#include <symtable.h>

void init_program(
  Program* prog, uint32_t init_len, uint32_t rod_len, uint32_t sym_len) {
  prog->len = init_len;
  prog->cap = init_len;

  // instruction array
  prog->ins = NULL;
  if (init_len) { prog->ins = memory(prog->ins, 0x0, init_len); }

  // readonly data
  init_rodata(&prog->rod, rod_len);

  // symbol data
  init_symtable(&prog->stb, sym_len);
}

void write_ins(Program* prog, uint8_t ins) {
  if (prog->len == prog->cap) {
    uint32_t size = GROW_ARRAY(prog->cap);
    prog->ins     = REALLOC_ARRAY(prog->ins, uint8_t, prog->cap, size);
    prog->cap     = size;
  }

  prog->ins[prog->len] = ins;
  prog->len++;
}

uint32_t write_rod(Program* prog, Value val) {
  rodata_write(&prog->rod, val);
  return prog->rod.len - 1;
}

uint32_t write_sym(Program* prog, Symbol sym) {
  symtable_write(&prog->stb, sym);
  return prog->stb.len - 1;
}

void free_program(Program* prog) {
  free_rodata(&prog->rod);
  free_symtable(&prog->stb);
  FREE_ARRAY(prog->ins, uint8_t, prog->cap);
}