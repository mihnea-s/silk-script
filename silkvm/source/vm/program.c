#include <mem.h>
#include <program.h>

void init_program(Program* prog) {
  prog->len  = 0;
  prog->cnks = NULL;
}

void free_program(Program* prog) {
  for (int i = 0; i < prog->len; i++) {
    free_chunk(&prog->cnks[i]);
  }

  release(prog->cnks, prog->len * sizeof(Chunk));
}