#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <chunk.h>
#include <file_exec.h>
#include <mem.h>
#include <program.h>
#include <value.h>

/*
 *  bytecode file:
 *    - "SILKVM"
 *    - chunk-count, 2byte, bigendian
 *      - constant-count, 1byte
 *        - constant
 *      - instruction-count, 2byte, bigendian
 *        - instruction, 1byte
 *    - "SVMEND"
 *
 */

static const char* header = "SVMEXE";
static const char* footer = "SVMEND";

#define SET_ERR(to_what)                                                       \
  if (err) { *err = to_what; }

#define ERR_ON_EOF                                                             \
  if (feof(f)) {                                                               \
    SET_ERR("malformed silk executable");                                      \
    return;                                                                    \
  }

#define READ_BYTE_IN(x) ERR_ON_EOF uint8_t x = getc(f);

#define READ_WORD_IN(x)                                                        \
  ERR_ON_EOF uint16_t x;                                                       \
  fread(&x, sizeof(uint16_t), 1, f);

#define READ_VALUE_IN(v)                                                       \
  Value v;                                                                     \
  do {                                                                         \
    READ_BYTE_IN(type);                                                        \
    v.type = type;                                                             \
    switch (type) {                                                            \
      case T_INT: {                                                            \
        fread(&v.as.integer, sizeof(int64_t), 1, f);                           \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_REAL: {                                                           \
        fread(&v.as.real, sizeof(double), 1, f);                               \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_STR: {                                                            \
        long start = ftell(f);                                                 \
                                                                               \
        while (true) {                                                         \
          READ_BYTE_IN(ch);                                                    \
          if (ch == 0x0) break;                                                \
        }                                                                      \
                                                                               \
        long str_len = ftell(f) - start;                                       \
        fseek(f, start, SEEK_SET);                                             \
        char* str = memory(NULL, 0, str_len);                                  \
        fread(str, sizeof(char), str_len, f);                                  \
                                                                               \
        v.as.string = str;                                                     \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_BOOL: {                                                           \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_VID: {                                                            \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_OBJ: {                                                            \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_REF: {                                                            \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  } while (false);

void read_file(const char* file, Program* prog, const char** err) {
  FILE* f = fopen(file, "r");
  if (!f) {
    SET_ERR("could not find file");
    return;
  }

  for (int i = 0; i < strlen(header); i++) {
    if (feof(f) || getc(f) != header[i]) {
      SET_ERR("file is not a silk executable");
      return;
    }
  }

  READ_WORD_IN(chunk_count);

  Chunk* prg = memory(NULL, 0, chunk_count * sizeof(Chunk));

  for (Chunk* cnk = prg; cnk < prg + chunk_count; cnk++) {
    init_chunk(cnk);

    READ_BYTE_IN(constant_count);

    for (; constant_count; constant_count--) {
      READ_VALUE_IN(constant_value);
      constant(cnk, constant_value);
    }

    READ_WORD_IN(instruction_count);

    for (; instruction_count; instruction_count--) {
      READ_BYTE_IN(instruction);
      write_ins(cnk, instruction);
    }
  }

  for (int i = 0; i < strlen(footer); i++) {
    if (feof(f) || getc(f) != footer[i]) {
      SET_ERR("malformed silk file");
      return;
    }
  }

  fclose(f);

  prog->cnks = prg;
  prog->len  = chunk_count;
}

#define WRITE_BYTE(c) fwrite(&c, sizeof(uint8_t), 1, f)
#define WRITE_WORD(c) fwrite(&c, sizeof(uint16_t), 1, f)
#define WRITE_VALUE(c)                                                         \
  do {                                                                         \
    WRITE_BYTE(c.type);                                                        \
    switch (c.type) {                                                          \
      case T_INT: {                                                            \
        fwrite(&c.as.integer, sizeof(int64_t), 1, f);                          \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_REAL: {                                                           \
        fwrite(&c.as.real, sizeof(double), 1, f);                              \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_STR: {                                                            \
        fwrite(c.as.string, sizeof(char), strlen(c.as.string), f);             \
        int null_byte = 0x0;                                                   \
        WRITE_BYTE(null_byte);                                                 \
        break;                                                                 \
      };                                                                       \
                                                                               \
      case T_BOOL: {                                                           \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_VID: {                                                            \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_OBJ: {                                                            \
        break;                                                                 \
      }                                                                        \
                                                                               \
      case T_REF: {                                                            \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  } while (false)

void write_file(const char* file, Program* prog, const char** err) {
  FILE* f = fopen(file, "w");
  if (!f) {
    SET_ERR("could not find file");
    return;
  }

  fwrite(header, strlen(header), 1, f);

  // chunk count
  WRITE_WORD(prog->len);

  for (size_t i = 0; i < prog->len; i++) {
    Chunk* cnk = &prog->cnks[i];

    uint8_t constant_count = cnk->constants.len;

    WRITE_BYTE(constant_count);

    for (size_t i = 0; i < constant_count; i++) {
      WRITE_VALUE(cnk->constants.vals[i]);
    }

    uint16_t instruction_count = cnk->len;

    WRITE_WORD(instruction_count);

    for (size_t i = 0; i < instruction_count; i++) {
      WRITE_BYTE(cnk->codes[i]);
    }
  }

  fwrite(footer, strlen(footer), 1, f);
  fclose(f);
}