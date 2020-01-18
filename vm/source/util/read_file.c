#include <stdio.h>
#include <string.h>

#include <util/message.h>
#include <util/read_file.h>

#include <vm/chunk.h>
#include <vm/mem.h>

/*
 *  bytecode file:
 *    - "SILKVM"
 *    - chunk-count, 4byte, bigendian
 *      - constant-count, 1byte
 *        - constant, 4byte, bigendian
 *      - instruction-count, 4byte, bigendian
 *        - instruction, 1byte
 *    - "SVMEND"
 *
 */

static const char *header = "SILKVM";
static const char *footer = "SVMEND";

#define NULL_ON_EOF                                                            \
  if (feof(f)) {                                                               \
    print_err("malformed silk executable");                                    \
    return NULL;                                                               \
  }

#define BYTE(x) NULL_ON_EOF int x = getc(f);
#define WORD(x) NULL_ON_EOF int x = getw(f);

Chunk *read_file(const char *file, int *count) {
  FILE *f = fopen(file, "r");
  if (!f) {
    print_err("could not find file '%s'", file);
    return NULL;
  }

  for (int i = 0; i < strlen(header); i++) {
    if (feof(f) || getc(f) != header[i]) {
      print_err("file is not a silk executable");
      return NULL;
    }
  }

  WORD(chunk_count);

  Chunk *prg = memory(NULL, 0, chunk_count * sizeof(Chunk));

  for (Chunk *cnk = prg; cnk < prg + chunk_count; cnk++) {
    init_chunk(cnk);

    BYTE(constant_count);

    for (; constant_count; constant_count--) {
      WORD(constant_value);
      constant(cnk, constant_value);
    }

    WORD(instruction_count);

    for (; instruction_count; instruction_count--) {
      BYTE(instruction);
      write_ins(cnk, instruction);
    }
  }

  for (int i = 0; i < strlen(footer); i++) {
    if (feof(f) || getc(f) != footer[i]) {
      print_err("malformed silk file");
      return NULL;
    }
  }

  (*count) = chunk_count;
  return prg;
}