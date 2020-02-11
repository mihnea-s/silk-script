#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <chunk.h>
#include <disas.h>
#include <opcode.h>
#include <value.h>

size_t single(size_t ofst, const char *name) {
  printf("0x%03zx %s\n", ofst, name);
  return ofst + 1;
}

size_t load_val(Chunk *cnk, size_t ofst) {
  uint8_t val_ofst = cnk->codes[ofst + 1];
  printf("0x%03zx VAL 0x%02d (", ofst, val_ofst);
  print_value(cnk->constants.vals[val_ofst]);
  printf(")\n");
  return ofst + 2;
}

size_t instruction(Chunk *cnk, size_t ofst) {
  uint8_t ins = cnk->codes[ofst];
  switch (ins) {

  case RET:
    return single(ofst, "RET");

  case VAL:
    return load_val(cnk, ofst);

  case NEG:
    return single(ofst, "NEG");

  case ADD:
    return single(ofst, "ADD");

  case SUB:
    return single(ofst, "SUB");

  case MUL:
    return single(ofst, "MUL");

  case DIV:
    return single(ofst, "DIV");

  default:
    return single(ofst, "???");
  }
};

void disassemble(Chunk *cnks, int cnk_count, const char *name) {
  printf("~~~~ %-10s ~~~~\n", name);
  for (Chunk *cnk = cnks; cnk < cnks + cnk_count; cnk++) {
    printf("============begin==\n");
    for (size_t ofst = 0; ofst < cnk->len;) {
      ofst = instruction(cnk, ofst);
    }
    printf("==============end==\n");
  }
}