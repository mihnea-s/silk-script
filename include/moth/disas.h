#ifndef MOTHVM_DISAS_H
#define MOTHVM_DISAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <moth/program.h>

void disassemble(const char*, Program*);

#ifdef __cplusplus
}
#endif

#endif