#ifndef SILKVMEXE_READ_FILE_H
#define SILKVMEXE_READ_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "program.h"

void read_file(const char*, Program*, const char**);
void write_file(const char*, Program*, const char**);

#ifdef __cplusplus
}
#endif

#endif