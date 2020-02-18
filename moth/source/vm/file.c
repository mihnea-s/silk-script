#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <file.h>
#include <macros.h>
#include <mem.h>
#include <object.h>
#include <program.h>
#include <string.h>
#include <value.h>

/*
 *  bytecode file:
 *   - "SILKEXECUTABLE"
 *   - version, 2 byte
 *   - instruction count [n], 4 bytes
 *   - read only data count [m], 4 bytes
 *   - symbol count [s], 4 bytes
 *     - [n] instructions, 1 byte each
 *     - [m] read only values, variable size
 *     - [s] symbols, variable size
 *   - checksum, 4 bytes
 *   - "SILKEND"
 */

static const char*    header  = "SILKEXECUTABLE";
static const uint16_t version = 0;
static const char*    footer  = "SILKEND";

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

static uint8_t read_u8(FILE* f) {
  return fgetc(f);
}

static uint16_t read_u16(FILE* f) {
  uint16_t x    = 0;
  size_t   read = fread(&x, sizeof(uint16_t), 1, f);
  if (read != 1) return 0x0;
  SWAP_IF_BIG_ENDIAN(x);
  return x;
}

static uint32_t read_u32(FILE* f) {
  uint32_t x    = 0;
  size_t   read = fread(&x, sizeof(uint32_t), 1, f);
  if (read != 1) return 0x0;
  SWAP_IF_BIG_ENDIAN(x);
  return x;
}

static int64_t read_i64(FILE* f) {
  int64_t x    = 0;
  size_t  read = fread(&x, sizeof(int64_t), 1, f);
  if (read != 1) return 0x0;
  SWAP_IF_BIG_ENDIAN(x);
  return x;
}

static double read_dbl(FILE* f) {
  double x    = 0;
  size_t read = fread(&x, sizeof(double), 1, f);
  if (read != 1) return 0.0;
  SWAP_IF_BIG_ENDIAN(x);
  return x;
}

static char* read_str(FILE* f) {
  if (feof(f)) return NULL;

  long start = ftell(f);

  while (true) {
    if (feof(f)) return NULL;
    if (read_u8(f) == 0x0) break;
  }

  long  str_len = ftell(f) - start;
  char* str     = memory(NULL, 0, str_len * sizeof(char));

  fseek(f, start, SEEK_SET);
  size_t read = fread(str, sizeof(char), str_len, f);

  if (read != str_len) {
    release(str, str_len * sizeof(char));
    return NULL;
  }

  return str;
}

static void read_value(Value* x, FILE* f, const char** err) {
  MALFORMED_EOF();
  x->type = read_u8(f);

  switch (x->type) {

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

    case T_STR: {
      MALFORMED_EOF();
      x->as.string = read_str(f);
      break;
    }

    case T_BOOL: {
      MALFORMED_EOF();
      x->as.boolean = read_u8(f);
      break;
    }

    case T_VID: {
      break;
    }

    case T_OBJ: {
      MALFORMED_EOF();
      ObjType type = read_u8(f);

      switch (type) {
        case O_FUNCTION: {
          MALFORMED_EOF();
          uint32_t len = read_u32(f);

          ObjectFunction* fct =
            memory(NULL, 0x0, sizeof(ObjectFunction) + sizeof(uint8_t) * len);

          fct->obj.type = type;
          fct->len      = len;
          x->as.object  = (Object*)fct;
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

void read_file(const char* file, Program* prog, const char** err) {
  FILE* f = fopen(file, "r");

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
    Symbol* sym = prog->stb.arr + i;
    sym->str    = read_str(f);
    sym->hash   = hash(sym->str);
  }

  uint32_t checksum = read_u32(f);

  for (size_t i = 0; i < strlen(footer); i++) {
    if (feof(f) || getc(f) != footer[i]) {
      MALFORMED();
      return;
    }
  }

  fclose(f);
}

#define WRITE_U8(c) fwrite(&c, sizeof(uint8_t), 1, f)

#define WRITE_U16(c) SWAP_IF_BIG_ENDIAN(c) fwrite(&c, sizeof(uint16_t), 1, f)

#define WRITE_U32(c)                                                           \
  SWAP_IF_BIG_ENDIAN(c);                                                       \
  fwrite(&c, sizeof(uint32_t), 1, f)

#define WRITE_I64(c)                                                           \
  SWAP_IF_BIG_ENDIAN(c);                                                       \
  fwrite(&c, sizeof(int64_t), 1, f)

#define WRITE_DBL(c)                                                           \
  SWAP_IF_BIG_ENDIAN(c);                                                       \
  fwrite(&c, sizeof(double), 1, f)

#define WRITE_STR(c) fwrite(c, sizeof(char), strlen(c), f)

#define WRITE_VALUE(c)                                                         \
  do {                                                                         \
    WRITE_U8(c.type);                                                          \
    switch (c.type) {                                                          \
      case T_INT: {                                                            \
        WRITE_I64(c.as.integer);                                               \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_REAL: {                                                           \
        WRITE_DBL(c.as.real);                                                  \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_STR: {                                                            \
        WRITE_STR(c.as.string);                                                \
        int null_byte = 0x0;                                                   \
        WRITE_U8(null_byte);                                                   \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_BOOL: {                                                           \
        WRITE_U8(c.as.boolean);                                                \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_VID: {                                                            \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_OBJ: {                                                            \
        WRITE_U8(c.as.object->type);                                           \
        switch (c.as.object->type) {                                           \
          case O_FUNCTION: {                                                   \
            ObjectFunction* fct = OBJ_FCT(c.as.object);                        \
            WRITE_U32(fct->len);                                               \
            fwrite(fct->bytes, sizeof(uint8_t), fct->len, f);                  \
            break;                                                             \
          }                                                                    \
                                                                               \
          default: break;                                                      \
        }                                                                      \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  } while (false)

#define WRITE_SYMBOL(c)                                                        \
  do {                                                                         \
    WRITE_STR(c.str);                                                          \
    int null_byte = 0x0;                                                       \
    WRITE_U8(null_byte);                                                       \
  } while (false)

void write_file(const char* file, Program* prog, const char** err) {
  FILE* f = fopen(file, "w");
  if (!f) {
    SET_ERR("could not find file");
    return;
  }

  fwrite(header, strlen(header), 1, f);

  uint16_t ver = version;
  WRITE_U16(ver);

  WRITE_U32(prog->len);
  WRITE_U32(prog->rod.len);
  WRITE_U32(prog->stb.len);

  for (uint32_t i = 0; i < prog->len; i++) {
    WRITE_U8(prog->bytes[i]);
  }

  for (uint32_t i = 0; i < prog->rod.len; i++) {
    WRITE_VALUE(prog->rod.arr[i]);
  }

  for (uint32_t i = 0; i < prog->stb.len; i++) {
    WRITE_SYMBOL(prog->stb.arr[i]);
  }

  uint32_t checksum = 0;

  WRITE_U32(checksum);

  fwrite(footer, strlen(footer), 1, f);
  fclose(f);
}