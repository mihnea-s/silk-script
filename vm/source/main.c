#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <util/cli.h>
#include <util/message.h>
#include <vm/file_exec.h>
#include <vm/chunk.h>
#include <vm/disas.h>
#include <vm/opcode.h>
#include <vm/vm.h>

int main(int argc, const char* argv[]) {
  const char** end = argv + argc;
  const char** arg = argv + 1;

  while (arg != end) {
    if (!param(*arg)) { break; }

    if (param_str(*arg, "--help", "-h")) {
      print_hlp();
      exit(0);
    }

    if (param_str(*arg, "--disassemble", "-d")) {
      param_opt_set(PARAM_DISSAS);
      arg++;
      continue;
    }

    if (param_str(*arg, "--print-stack", "-s")) {
      param_opt_set(PARAM_PSTACK);
      arg++;
      continue;
    }

    print_err("invalid parameter '%s'", *arg);
    exit(1);
  }

  if (arg == end) {
    print_err("no file to run");
    exit(1);
  }

  int    cnk_count;
  const char* err = NULL;
  Chunk* cnks = read_file(*arg, &cnk_count, &err);

  if (err) {
    print_err(err);
    exit(1);
  }

  if (param_opt_get(PARAM_DISSAS)) {
    disassemble(cnks, cnk_count, *arg);
    exit(0);
  }

  VM vm;
  init_vm(&vm);

  run(&vm, cnks);

  free_chunk(cnks);
  free_vm(&vm);

  return 0;
}