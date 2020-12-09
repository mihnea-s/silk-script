/**
 *  Here's the structure of the bytecode file:
 *   - "SILKEXE"
 *   - version, 2 byte
 *   - instruction count [n], 4 bytes
 *   - read only data count [m], 4 bytes
 *   - symbol count [s], 4 bytes
 *   ~~ [n] instructions, 1 byte each
 *   ~~ [m] read only values, variable size
 *   ~~ [s] symbols, variable size
 *   - checksum, 4 bytes
 *   - "SILKEND"
 */
#include <moth/file.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <moth/macros.h>
#include <moth/mem.h>
#include <moth/object.h>
#include <moth/program.h>
#include <moth/value.h>

static const char *   header  = "SILKEXE";
static const uint16_t version = 0;
static const char *   footer  = "SILKEND";

uint32_t checksum(Program *prog) {
  uint32_t x = 2166136261u;

  for (size_t i = 0; i < prog->len; i++) {
    x *= 16777619u;
    x ^= prog->bytes[i];
  }

  for (size_t i = 0; i < prog->stb.len; i++) {
    x *= 16777619u;
    x ^= hash(prog->stb.arr[i].str);
  }

  return x;
}

#define SET_ERR(to_what)                                                       \
  if (err) { *err = to_what; }

#define MALFORMED() SET_ERR("malformed silk executable");

#define MALFORMED_EOF()                                                        \
  if (feof(f)) {                                                               \
    MALFORMED();                                                               \
    return;                                                                    \
  }

#define SWAP_IF_BIG_ENDIAN(x)                                                  \
  if (IS_BIG_ENDIAN) { x = SWAP_BYTES(x); }

static uint8_t read_u8(FILE *f) {
  return fgetc(f);
}

#define DEFINE_READ(FUNCTION, TYPE)                                            \
  static TYPE FUNCTION(FILE *f) {                                              \
    TYPE   x    = 0;                                                           \
    size_t read = fread(&x, sizeof(TYPE), 1, f);                               \
    if (read != 1) return 0;                                                   \
    SWAP_IF_BIG_ENDIAN(x);                                                     \
    return x;                                                                  \
  }

DEFINE_READ(read_u16, uint16_t);
DEFINE_READ(read_u32, uint32_t);
DEFINE_READ(read_chr, wchar_t);
DEFINE_READ(read_i64, int64_t);

static double read_dbl(FILE *f) {
  double integral = read_u32(f);
  double fraction = read_u32(f);
  return integral + (fraction / 10E+10);
}

static char *read_str(FILE *f) {
  if (feof(f)) return NULL;

  long start = ftell(f);

  while (true) {
    if (feof(f)) return NULL;
    if (read_u8(f) == 0x0) break;
  }

  long  str_len = ftell(f) - start;
  char *str     = memory(NULL, 0, str_len * sizeof(char));

  fseek(f, start, SEEK_SET);
  size_t read = fread(str, sizeof(char), str_len, f);

  if (read != str_len) {
    release(str, str_len * sizeof(char));
    return NULL;
  }

  return str;
}

static void read_value(Value *x, FILE *f, const char **err) {
  MALFORMED_EOF();
  x->type = read_u8(f);

  switch (x->type) {

    case T_VOID: {
      break;
    }

    case T_BOOL: {
      MALFORMED_EOF();
      x->as.boolean = read_u8(f);
      break;
    }

    case T_INT: {
      MALFORMED_EOF();
      x->as.integer = read_i64(f);
      break;
    }

    case T_REAL: {
      MALFORMED_EOF();
      x->as.real = read_dbl(f);
      break;
    }

    case T_CHAR: {
      MALFORMED_EOF();
      x->as.charac = read_chr(f);
      break;
    }

    case T_STR: {
      MALFORMED_EOF();
      x->as.string = read_str(f);
      break;
    }

    case T_OBJ: {
      MALFORMED_EOF();
      ObjType type = read_u8(f);

      switch (type) {
        case O_FUNCTION: {
          MALFORMED_EOF();
          uint32_t len = read_u32(f);

          ObjectFunction *fct =
            memory(NULL, 0x0, sizeof(ObjectFunction) + sizeof(uint8_t) * len);

          fct->obj.type = type;
          fct->len      = len;
          x->as.object  = (Object *)fct;
          size_t read   = fread(fct->bytes, sizeof(uint8_t), len, f);

          if (read != len) SET_ERR("malformed silk executable");
          break;
        }

        default: break;
      }
      break;
    }
  }
}

void read_file(const char *file, Program *prog, const char **err) {
  FILE *f = fopen(file, "r");

  if (!f) {
    SET_ERR("file not found");
    return;
  } else {
    SET_ERR(NULL);
  }

  for (size_t i = 0; i < strlen(header); i++) {
    if (feof(f) || getc(f) != header[i]) {
      MALFORMED();
      return;
    }
  }

  if (read_u16(f) != version) {
    MALFORMED();
    return;
  }

  uint32_t ins_len = read_u32(f);
  uint32_t rod_len = read_u32(f);
  uint32_t sym_len = read_u32(f);
  init_program(prog, ins_len, rod_len, sym_len);

  for (uint32_t i = 0; i < ins_len; i++) {
    MALFORMED_EOF();
    prog->bytes[i] = read_u8(f);
  }

  for (uint32_t i = 0; i < rod_len; i++) {
    MALFORMED_EOF();
    read_value(prog->rod.arr + i, f, err);
    if (*err) return;
  }

  for (uint32_t i = 0; i < sym_len; i++) {
    MALFORMED_EOF();
    Symbol *sym = prog->stb.arr + i;
    sym->str    = read_str(f);
    sym->hash   = hash(sym->str);
  }

  // Check the checksum of the program
  if (read_u32(f) != checksum(prog)) MALFORMED();

  // Finally check the footer of the file
  for (size_t i = 0; i < strlen(footer); i++) {
    if (feof(f) || getc(f) != footer[i]) {
      MALFORMED();
      return;
    }
  }

  fclose(f);
}

#define DEFINE_WRITE(FUNCTION, TYPE)                                           \
  static void FUNCTION(TYPE t, FILE *f) {                                      \
    SWAP_IF_BIG_ENDIAN(t);                                                     \
    fwrite(&t, sizeof(TYPE), 1, f);                                            \
  }

DEFINE_WRITE(write_u8, uint8_t);
DEFINE_WRITE(write_u16, uint16_t);
DEFINE_WRITE(write_u32, uint32_t);
DEFINE_WRITE(write_chr, wchar_t);
DEFINE_WRITE(write_i64, int64_t);

static void write_dbl(double d, FILE *f) {
  double ingr, frac;
  frac = modf(d, &ingr);

  uint32_t integral = ingr;
  uint32_t fraction = frac * 10E+10;

  write_u32(integral, f);
  write_u32(fraction, f);
}

static void write_str(const char *s, FILE *f) {
  fwrite(s, sizeof(char), strlen(s), f);
  write_u8(0x0, f);
}

static void write_obj(Object *obj, FILE *f) {
  write_u8(obj->type, f);
  switch (obj->type) {

    case O_FUNCTION: {
      ObjectFunction *fct = OBJ_FCT(obj);
      write_u32(fct->len, f);
      fwrite(fct->bytes, sizeof(uint8_t), fct->len, f);
      break;
    }

    default: break;
  }
}

static void write_value(Value v, FILE *f) {
  write_u8(v.type, f);
  switch (v.type) {
    case T_BOOL: return write_u8(v.as.boolean, f);
    case T_INT: return write_i64(v.as.integer, f);
    case T_REAL: return write_dbl(v.as.real, f);
    case T_CHAR: return write_chr(v.as.charac, f);
    case T_STR: return write_str(v.as.string, f);
    case T_OBJ: return write_obj(v.as.object, f);
    default: return;
  }
}

static void write_symbol(Symbol sy, FILE *f) {
  write_str(sy.str, f);
}

void write_file(const char *file, Program *prog, const char **err) {
  FILE *f = fopen(file, "w");
  if (!f) {
    SET_ERR("could not find file");
    return;
  }

  fwrite(header, strlen(header), 1, f);
  write_u16(version, f);

  write_u32(prog->len, f);
  write_u32(prog->rod.len, f);
  write_u32(prog->stb.len, f);

  for (uint32_t i = 0; i < prog->len; i++) {
    write_u8(prog->bytes[i], f);
  }

  for (uint32_t i = 0; i < prog->rod.len; i++) {
    write_value(prog->rod.arr[i], f);
  }

  for (uint32_t i = 0; i < prog->stb.len; i++) {
    write_symbol(prog->stb.arr[i], f);
  }

  // Write the checksum to the program
  write_u32(checksum(prog), f);

  fwrite(footer, strlen(footer), 1, f);
  fclose(f);
}