#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <util/cli.h>
#include <util/message.h>

int parameter_options_bits = 0;

bool param(const char* arg) {
  return strncmp(arg, "-", 1) == 0;
}

bool param_opt_get(int opt) {
  return parameter_options_bits & opt;
}

void param_opt_set(int opt) {
  parameter_options_bits |= opt;
}

bool param_str(const char* arg, const char* v1, const char* v2) {
  return strcmp(arg, v1) * strcmp(arg, v2) == 0;
}

void print_param(const char* v1, const char* v2, const char* what) {
  printf("\t" BOLD "%s" RESET ", " BOLD "%s" RESET "  ->  %s\n", v1, v2, what);
}

void print_hlp() {
  printf(BLUE BOLD "(?) Help" RESET ": silkvm usage:\n");
  print_param("--disassemble", "-d", "dissasemble file bytecode");
}
