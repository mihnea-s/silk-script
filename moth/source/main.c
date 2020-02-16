#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <util/cli.h>
#include <util/message.h>
#include <vm/disas.h>
#include <vm/file.h>
#include <vm/opcode.h>
#include <vm/program.h>
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

    print_err("invalid parameter '%s'", *arg);
    exit(1);
  }

  if (arg == end) {
    print_err("no file to run");
    exit(1);
  }

  if (arg + 1 != end) {
    print_err("invalid argument count");
    exit(1);
  }

  const char* file = *arg;
  const char* err  = NULL;

  Program prog;
  read_file(file, &prog, &err);

  if (err) {
    print_err(err);
    exit(1);
  }

  if (param_opt_get(PARAM_DISSAS)) {
    disassemble(file, &prog);
    exit(0);
  }

  VM vm;
  init_vm(&vm);
  run(&vm, &prog);

  int ret = vm.st;

  if (ret != STATUS_OK) {
    print_err("vm finished execution with error code: %d", ret);
  }

  free_vm(&vm);
  free_program(&prog);

  return ret;
}