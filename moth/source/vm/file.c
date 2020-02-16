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

#define ERR_ON_EOF                                                             \
  if (feof(f)) {                                                               \
    SET_ERR("malformed silk executable");                                      \
    return;                                                                    \
  }

#define SWAP_IF_BIG_ENDIAN(x)                                                  \
  if (IS_BIG_ENDIAN) { x = SWAP_BYTES(x); }

#define READ_U8(x) ERR_ON_EOF x = getc(f);

#define READ_U16(x)                                                            \
  ERR_ON_EOF                                                                   \
  fread(&x, sizeof(uint16_t), 1, f);                                           \
  SWAP_IF_BIG_ENDIAN(x)

#define READ_U32(x)                                                            \
  ERR_ON_EOF                                                                   \
  fread(&x, sizeof(uint32_t), 1, f);                                           \
  SWAP_IF_BIG_ENDIAN(x)

#define READ_I64(x)                                                            \
  ERR_ON_EOF                                                                   \
  fread(&x, sizeof(int64_t), 1, f);                                            \
  SWAP_IF_BIG_ENDIAN(x)

#define READ_DBL(x)                                                            \
  ERR_ON_EOF                                                                   \
  fread(&x, sizeof(double), 1, f);                                             \
  SWAP_IF_BIG_ENDIAN(x)

#define READ_STR(x)                                                            \
  ERR_ON_EOF                                                                   \
  do {                                                                         \
    long start = ftell(f);                                                     \
    char ch;                                                                   \
    while (true) {                                                             \
      READ_U8(ch);                                                             \
      if (ch == 0x0) break;                                                    \
    }                                                                          \
    long str_len = ftell(f) - start;                                           \
    fseek(f, start, SEEK_SET);                                                 \
    x = memory(NULL, 0, str_len * sizeof(char));                               \
    fread(x, sizeof(char), str_len, f);                                        \
  } while (false)

#define READ_VALUE(x)                                                          \
  do {                                                                         \
    READ_U8(x.type);                                                           \
    switch (x.type) {                                                          \
      case T_INT: {                                                            \
        READ_I64(x.as.integer);                                                \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_REAL: {                                                           \
        READ_DBL(x.as.real);                                                   \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_STR: {                                                            \
        READ_STR(x.as.string);                                                 \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_BOOL: {                                                           \
        READ_U8(x.as.boolean);                                                 \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_VID: {                                                            \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_OBJ: {                                                            \
        ObjType type;                                                          \
        READ_U8(type);                                                         \
        switch (type) {                                                        \
          case O_FUNCTION: {                                                   \
            uint32_t len;                                                      \
            READ_U32(len);                                                     \
                                                                               \
            ObjectFunction* fct = memory(                                      \
              NULL, 0x0, sizeof(ObjectFunction) + sizeof(uint8_t) * len);      \
                                                                               \
            fct->obj.type = type;                                              \
            fct->len      = len;                                               \
            x.as.object   = (Object*)fct;                                      \
            fread(fct->ins, sizeof(uint8_t), len, f);                          \
                                                                               \
            break;                                                             \
          }                                                                    \
                                                                               \
          default: break;                                                      \
        }                                                                      \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  } while (false)

#define READ_SYMBOL(x)                                                         \
  do {                                                                         \
    READ_STR(x.str);                                                           \
    x.hash = hash(x.str);                                                      \
  } while (false)

void read_file(const char* file, Program* prog, const char** err) {
  FILE* f = fopen(file, "r");
  if (!f) {
    SET_ERR("could not find file");
    return;
  }

  for (size_t i = 0; i < strlen(header); i++) {
    if (feof(f) || getc(f) != header[i]) {
      SET_ERR("file is not a silk executable");
      return;
    }
  }

  uint16_t ver;
  READ_U16(ver);

  if (ver != version) {
    SET_ERR("version error");
    return;
  }

  uint32_t ins_len, rod_len, sym_len;

  READ_U32(ins_len);
  READ_U32(rod_len);
  READ_U32(sym_len);

  init_program(prog, ins_len, rod_len, sym_len);

  for (uint32_t i = 0; i < ins_len; i++) {
    READ_U8(prog->ins[i]);
  }

  for (uint32_t i = 0; i < rod_len; i++) {
    READ_VALUE(prog->rod.vls[i]);
  }

  for (uint32_t i = 0; i < sym_len; i++) {
    READ_SYMBOL(prog->stb.syms[i]);
  }

  uint32_t checksum;

  READ_U32(checksum);

  for (size_t i = 0; i < strlen(footer); i++) {
    if (feof(f) || getc(f) != footer[i]) {
      SET_ERR("malformed silk file");
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
            fwrite(fct->ins, sizeof(uint8_t), fct->len, f);                    \
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
    WRITE_U8(prog->ins[i]);
  }

  for (uint32_t i = 0; i < prog->rod.len; i++) {
    WRITE_VALUE(prog->rod.vls[i]);
  }

  for (uint32_t i = 0; i < prog->stb.len; i++) {
    WRITE_SYMBOL(prog->stb.syms[i]);
  }

  uint32_t checksum = 0;

  WRITE_U32(checksum);

  fwrite(footer, strlen(footer), 1, f);
  fclose(f);
}