#include <silk/mothvm/disas.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <silk/mothvm/macros.h>
#include <silk/mothvm/object.h>
#include <silk/mothvm/opcode.h>
#include <silk/mothvm/program.h>
#include <silk/mothvm/value.h>

typedef struct {
  uint8_t*        codes;
  uint32_t        ofst;
  const Rodata*   rodata;
  const Symtable* symtab;
} DissasmInfo;

static void single(DissasmInfo* info, const char* name) {
  printf("0x%03x %s\n", info->ofst++, name);
}

static uint32_t read_address(DissasmInfo* info, int addr_sz) {
  uint32_t val_ofst = 0;

  for (int i = 0; i < addr_sz; i++) {
    val_ofst <<= 8;
    val_ofst |= info->codes[info->ofst + i];
  }

#if IS_BIG_ENDIAN
  val_ofst = SWAP_BYTES(val_ofst);
#endif

  return val_ofst;
}

static void move(DissasmInfo* info, const char* name, const char* op) {
  uint32_t ofst = info->ofst;

  info->ofst++;
  uint16_t index = read_address(info, 2);

  info->ofst += 2;
  printf("0x%03x %s %s [%d]\n", ofst, name, op, index);
}

static void immd_int(DissasmInfo* info) {
  uint32_t ofst = info->ofst;
  info->ofst++;

  union {
    uint64_t full;
    struct {
      uint32_t second;
      uint32_t first;
    } parts;
  } value;

  value.parts.first = read_address(info, 4);
  info->ofst += 4;

  value.parts.second = read_address(info, 4);
  info->ofst += 4;

  printf("0x%03x INT %+ld\n", ofst, value.full);
}

static void immd_rea(DissasmInfo* info) {
  uint32_t ofst = info->ofst;
  info->ofst++;

  union {
    double real;
    struct {
      uint32_t second;
      uint32_t first;
    } parts;
  } value;

  value.parts.first = read_address(info, 4);
  info->ofst += 4;

  value.parts.second = read_address(info, 4);
  info->ofst += 4;

  printf("0x%03x REA %.4lf\n", ofst, value.real);
}

static void immd_chr(DissasmInfo* info) {
  uint32_t ofst = info->ofst;

  info->ofst++;
  wchar_t chr = read_address(info, 4);

  info->ofst += 4;
  printf("0x%03x CHR '%lc'\n", ofst, chr);
}

static void load_val(DissasmInfo* info, int addr_sz) {
  uint32_t ofst = info->ofst;

  info->ofst++;
  uint32_t val_ofst = read_address(info, addr_sz);

  info->ofst += addr_sz;
  printf("0x%03x VAL%d 0x%02x (", ofst, addr_sz, val_ofst);
  print_value(info->rodata->arr[val_ofst]);
  printf(")\n");
}

static void symbol_op(DissasmInfo* info, const char* op, int addr_sz) {
  uint32_t ofst = info->ofst;

  info->ofst++;
  uint32_t sym_off = read_address(info, addr_sz);

  info->ofst += addr_sz;
  const char* sym_str = info->symtab->arr[sym_off].str;

  printf("0x%03x %s%d $%s [0x%02x]", ofst, op, addr_sz, sym_str, sym_off);
  printf("\n");
}

static void jump(DissasmInfo* info, const char* op, int dir) {
  uint32_t ofst = info->ofst;

  info->ofst++;
  int32_t off = dir * (int32_t)read_address(info, 2);

  info->ofst += 2;
  uint32_t landing = info->ofst + off;

  printf("0x%03x %s %+i (0x%03x)\n", ofst, op, off, landing);
}

static void call(DissasmInfo* info, const char* op) {
  uint32_t ofst = info->ofst;

  info->ofst++;
  uint8_t argc = info->codes[info->ofst++];

  printf("0x%03x %s #%d\n", ofst, op, argc);
}

static void frame(DissasmInfo* info, const char* op, int addr_sz) {
  uint32_t ofst = info->ofst;

  info->ofst++;
  uint32_t address = read_address(info, addr_sz);

  info->ofst += addr_sz;
  printf("0x%03x %s >=> 0x%03x\n", ofst, op, address);
}

static void instruction(DissasmInfo* info) {
  OpCode code = info->codes[info->ofst];
  switch (code) {
    case VM_FIN: return single(info, "FIN");
    case VM_NOP: return single(info, "NOP");
    case VM_GC: return single(info, "GC");
    case VM_DBG: return single(info, "DBG");

    case VM_POP: return single(info, "POP");
    case VM_PSH: return move(info, "PSH", "<-");
    case VM_STR: return move(info, "STR", "->");

    case VM_JMP: return jump(info, "JMP", 1);
    case VM_JPT: return jump(info, "JPT", 1);
    case VM_JPF: return jump(info, "JPF", 1);
    case VM_JBW: return jump(info, "JBW", -1);

    case VM_FRM: return frame(info, "FRM", 1);
    case VM_FRM2: return frame(info, "FRM", 2);
    case VM_FRM3: return frame(info, "FRM", 3);
    case VM_FRM4: return frame(info, "FRM", 4);

    case VM_CLO: return single(info, "CLO");
    case VM_CAL: return call(info, "CAL");
    case VM_PRO: return single(info, "PRO");
    case VM_RET: return single(info, "RET");

    case VM_INT: return immd_int(info);
    case VM_REA: return immd_rea(info);
    case VM_CHR: return immd_chr(info);

    case VM_VID: return single(info, "VID");
    case VM_TRU: return single(info, "TRU");
    case VM_FAL: return single(info, "FAL");

    case VM_ARR: return single(info, "ARR");
    case VM_DCT: return single(info, "DCT");

    case VM_VAL: return load_val(info, 1);
    case VM_VAL2: return load_val(info, 2);
    case VM_VAL3: return load_val(info, 3);
    case VM_VAL4: return load_val(info, 4);

    case VM_DEF: return symbol_op(info, "DEF", 1);
    case VM_DEF2: return symbol_op(info, "DEF", 2);
    case VM_DEF3: return symbol_op(info, "DEF", 3);
    case VM_DEF4: return symbol_op(info, "DEF", 4);

    case VM_GET: return symbol_op(info, "GET", 1);
    case VM_GET2: return symbol_op(info, "GET", 2);
    case VM_GET3: return symbol_op(info, "GET", 3);
    case VM_GET4: return symbol_op(info, "GET", 4);

    case VM_NEG: return single(info, "NEG");
    case VM_NOT: return single(info, "NOT");

    case VM_ADD: return single(info, "ADD");
    case VM_SUB: return single(info, "SUB");
    case VM_DIV: return single(info, "DIV");
    case VM_MUL: return single(info, "MUL");
    case VM_RIV: return single(info, "RIV");
    case VM_POW: return single(info, "POW");
    case VM_MOD: return single(info, "MOD");

    case VM_IDX: return single(info, "IDX");
    case VM_IDA: return single(info, "IDA");
    case VM_APP: return single(info, "APP");
    case VM_MRG: return single(info, "MRG");

    case VM_EQ: return single(info, "EQ");
    case VM_NEQ: return single(info, "NEQ");
    case VM_GT: return single(info, "GT");
    case VM_LT: return single(info, "LT");
    case VM_GTE: return single(info, "GTE");
    case VM_LTE: return single(info, "LTE");
  }
};

void disassemble(const char* name, const Program* prog) {
  DissasmInfo info;
  info.rodata = &prog->rod;
  info.symtab = &prog->stb;

  printf("%% %-10s: \n", name);
  for (Value* v = info.rodata->arr; v < info.rodata->arr + info.rodata->len; v++) {
    if (!IS_OBJ_FCT((*v))) continue;
    printf("~~ fct @ 0x%02lx ~~~~~~~~~~~\n", v - info.rodata->arr);

    info.codes = OBJ_FCT(v->as.object)->bytes;
    info.ofst  = 0;

    const uint32_t len = OBJ_FCT(v->as.object)->len;

    while (info.ofst < len) {
      instruction(&info);
    }

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  }

  info.codes = prog->bytes;
  info.ofst  = 0;

  printf("= main =================\n");
  while (info.ofst < prog->len) {
    instruction(&info);
  }
  printf("= end ==================\n");
}
