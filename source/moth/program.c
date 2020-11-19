#include <moth/program.h>

#include <stdint.h>

#include <moth/mem.h>
#include <moth/rodata.h>
#include <moth/symtable.h>

void init_program(
  Program* prog, uint32_t init_len, uint32_t rod_len, uint32_t sym_len) {
  prog->len   = init_len;
  prog->cap   = init_len;
  prog->bytes = init_len ? memory(NULL, 0, sizeof(uint8_t) * init_len) : NULL;

  // readonly data
  init_rodata(&prog->rod, rod_len);

  // symbol data
  init_symtable(&prog->stb, sym_len);
}

void write_byte(Program* prog, uint8_t byte) {
  if (prog->len == prog->cap) {
    uint32_t new_cap = GROW_CAP(prog->cap);

    size_t new_size = sizeof(uint8_t) * new_cap;
    size_t old_size = sizeof(uint8_t) * prog->cap;

    prog->cap   = new_cap;
    prog->bytes = memory(prog->bytes, old_size, new_size);
  }

  prog->bytes[prog->len] = byte;
  prog->len++;
}

uint32_t write_rodata(Program* prog, Value val) {
  rodata_write(&prog->rod, val);
  return prog->rod.len - 1;
}

uint32_t write_symtable(Program* prog, Symbol sym) {
  symtable_write(&prog->stb, sym);
  return prog->stb.len - 1;
}

void free_program(Program* prog) {
  free_rodata(&prog->rod);
  free_symtable(&prog->stb);
  release(prog->bytes, sizeof(uint8_t) * prog->cap);
}