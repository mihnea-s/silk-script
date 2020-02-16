#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <disas.h>
#include <macros.h>
#include <object.h>
#include <opcode.h>
#include <program.h>
#include <value.h>

typedef struct {
  Rodata*   rodata;
  Symtable* symtab;
  uint8_t*  codes;
  uint32_t  ofst;
} DissasmInfo;

static void single(DissasmInfo* info, const char* name) {
  printf("0x%03x %s\n", info->ofst, name);
  info->ofst++;
}

static void move(DissasmInfo* info, const char* name, const char* op) {
  uint8_t index = info->codes[info->ofst + 1];
  printf("0x%03x %s %s [%d]\n", info->ofst, name, op, index);
  info->ofst += 2;
}

static uint32_t read_address(DissasmInfo* info, int addr_sz) {
  uint32_t val_ofst = 0;
  for (int i = 0; i < addr_sz; i++) {
    val_ofst <<= 8;
    val_ofst |= info->codes[info->ofst + i];
  }
  return val_ofst;
}

static void load_val(DissasmInfo* info, int addr_sz) {
  info->ofst++;

  uint32_t val_ofst = read_address(info, addr_sz);

#if IS_BIG_ENDIAN
  val_ofst = SWAP_BYTES(val_ofst);
#endif

  printf("0x%03x VAL%d 0x%02x (", info->ofst, addr_sz, val_ofst);
  print_value(info->rodata->vls[val_ofst]);
  printf(")\n");

  info->ofst += addr_sz;
}

static void symbol_op(DissasmInfo* info, const char* op, int addr_sz) {
  info->ofst++;

  uint32_t sym_off = read_address(info, addr_sz);

#if IS_BIG_ENDIAN
  sym_off = SWAP_BYTES(sym_off);
#endif

  const char* sym_str = info->symtab->syms[sym_off].str;
  printf("0x%03x %s%d $%s [0x%02x]", info->ofst, op, addr_sz, sym_str, sym_off);
  printf("\n");

  info->ofst += addr_sz;
}

static void jump(DissasmInfo* info, const char* op, int dir) {
  info->ofst++;

  uint32_t off = read_address(info, 2);

#if IS_BIG_ENDIAN
  off = SWAP_BYTES(off);
#endif

  uint32_t landing = info->ofst + 2 + off * dir;
  printf("0x%03x %s +%u (0x%03x)\n", info->ofst, op, off, landing);
  info->ofst += 2;
}

static void frame(DissasmInfo* info, const char* op, int addr_sz) {
  info->ofst++;

  uint32_t address = read_address(info, addr_sz);

#if IS_BIG_ENDIAN
  address = SWAP_BYTES(address);
#endif

  printf("0x%03x %s >=> 0x%03x\n", info->ofst, op, address);
  info->ofst += addr_sz;
}

static void instruction(DissasmInfo* info) {
  uint8_t ins = info->codes[info->ofst];
  switch (ins) {
    case VM_FIN: return single(info, "FIN");

    case VM_FRM: return frame(info, "FRM", 1);
    case VM_FRM2: return frame(info, "FRM", 2);
    case VM_FRM3: return frame(info, "FRM", 3);
    case VM_FRM4: return frame(info, "FRM", 4);

    case VM_CAL: return single(info, "CAL");
    case VM_RET: return single(info, "RET");

    case VM_POP: return single(info, "POP");
    case VM_PSH: return move(info, "PSH", "<-");
    case VM_STR: return move(info, "STR", "->");

    case VM_JMP: return jump(info, "JMP", 1);
    case VM_JPT: return jump(info, "JPT", 1);
    case VM_JPF: return jump(info, "JPF", 1);
    case VM_JBW: return jump(info, "JBW", -1);

    case VM_VAL: return load_val(info, 1);
    case VM_VAL2: return load_val(info, 2);
    case VM_VAL3: return load_val(info, 3);
    case VM_VAL4: return load_val(info, 4);

    case VM_DEF: return symbol_op(info, "DEF", 1);
    case VM_DEF2: return symbol_op(info, "DEF", 2);
    case VM_DEF3: return symbol_op(info, "DEF", 3);
    case VM_DEF4: return symbol_op(info, "DEF", 4);

    case VM_SYM: return symbol_op(info, "SYM", 1);
    case VM_SYM2: return symbol_op(info, "SYM", 2);
    case VM_SYM3: return symbol_op(info, "SYM", 3);
    case VM_SYM4: return symbol_op(info, "SYM", 4);

    case VM_ASN: return symbol_op(info, "ASN", 1);
    case VM_ASN2: return symbol_op(info, "ASN", 2);
    case VM_ASN3: return symbol_op(info, "ASN", 3);
    case VM_ASN4: return symbol_op(info, "ASN", 4);

    case VM_NEG: return single(info, "NEG");
    case VM_NOT: return single(info, "NOT");
    case VM_TYP: return single(info, "TYP");

    case VM_ADD: return single(info, "ADD");
    case VM_SUB: return single(info, "SUB");
    case VM_DIV: return single(info, "DIV");
    case VM_MUL: return single(info, "MUL");
    case VM_RIV: return single(info, "RIV");
    case VM_POW: return single(info, "POW");
    case VM_MOD: return single(info, "MOD");

    case VM_NOP: return single(info, "NOP");
    case VM_VID: return single(info, "VID");
    case VM_TRU: return single(info, "TRU");
    case VM_FAL: return single(info, "FAL");

    case VM_IS: return single(info, "IS");
    case VM_ISN: return single(info, "ISN");

    case VM_EQ: return single(info, "EQ");
    case VM_NEQ: return single(info, "NEQ");
    case VM_GT: return single(info, "GT");
    case VM_LT: return single(info, "LT");
    case VM_GTE: return single(info, "GTE");
    case VM_LTE: return single(info, "LTE");

    case VM_PI: return single(info, "PI");
    case VM_TAU: return single(info, "TAU");
    case VM_EUL: return single(info, "EUL");

    default: return single(info, "???");
  }
};

void disassemble(const char* name, Program* prog) {
  DissasmInfo info;
  info.rodata = &prog->rod;
  info.symtab = &prog->stb;

  printf("@ %-10s: \n", name);
  for (Value* v = info.rodata->vls; v < info.rodata->vls + info.rodata->len;
       v++) {
    if (!IS_OBJ_FCT((*v))) continue;
    printf("~~ fct @ 0x%02lx ~~~~~~~~~~~\n", v - info.rodata->vls);

    info.codes = OBJ_FCT(v)->ins;
    info.ofst  = 0;

    while (info.ofst < OBJ_FCT(v)->len) {
      instruction(&info);
    }

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  }

  info.codes = prog->ins;
  info.ofst  = 0;

  printf("= main =================\n");
  while (info.ofst < prog->len) {
    instruction(&info);
  }
  printf("= end ==================\n");
}