#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <chunk.h>
#include <disas.h>
#include <opcode.h>
#include <value.h>

size_t single(size_t ofst, const char* name) {
  printf("0x%03zx %s\n", ofst, name);
  return ofst + 1;
}

size_t load_val(Chunk* cnk, size_t ofst) {
  uint8_t val_ofst = cnk->codes[ofst + 1];
  printf("0x%03zx VAL 0x%02d (", ofst, val_ofst);
  print_value(cnk->constants.vals[val_ofst]);
  printf(")\n");
  return ofst + 2;
}

size_t instruction(Chunk* cnk, size_t ofst) {
  uint8_t ins = cnk->codes[ofst];
  switch (ins) {

    case VM_RET: return single(ofst, "RET");
    case VM_VAL: return load_val(cnk, ofst);

    case VM_NEG: return single(ofst, "NEG");

    case VM_ADD: return single(ofst, "ADD");
    case VM_SUB: return single(ofst, "SUB");
    case VM_DIV: return single(ofst, "DIV");
    case VM_MUL: return single(ofst, "MUL");
    case VM_RIV: return single(ofst, "RIV");
    case VM_POW: return single(ofst, "POW");
    case VM_MOD: return single(ofst, "MOD");

    case VM_NOP: return single(ofst, "NOP");
    case VM_VID: return single(ofst, "VID");
    case VM_TRU: return single(ofst, "TRU");
    case VM_FAL: return single(ofst, "FAL");

    default: return single(ofst, "???");
  }
};

void disassemble(Chunk* cnks, int cnk_count, const char* name) {
  printf("~~~~ %-10s ~~~~\n", name);
  for (Chunk* cnk = cnks; cnk < cnks + cnk_count; cnk++) {
    printf("============begin==\n");
    for (size_t ofst = 0; ofst < cnk->len;) {
      ofst = instruction(cnk, ofst);
    }
    printf("==============end==\n");
  }
}