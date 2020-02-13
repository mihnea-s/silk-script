#ifndef SILKVM_DISAS_H
#define SILKVM_DISAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "program.h"

void disassemble(const char*, Program*);

#ifdef __cplusplus
}
#endif

#endif