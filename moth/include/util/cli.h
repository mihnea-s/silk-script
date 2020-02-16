#ifndef MOTHEXE_CLI_H
#define MOTHEXE_CLI_H

#include <stdbool.h>

#define PARAM_DISSAS 1 << 0
#define PARAM_PSTACK 1 << 1

bool param(const char*);
bool param_opt_get(int);
void param_opt_set(int);
bool param_str(const char*, const char*, const char*);

void print_hlp();

#endif