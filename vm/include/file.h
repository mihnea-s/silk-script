#ifndef MOTHVM_FILE_H
#define MOTHVM_FILE_H

#include <bits/stdint-uintn.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <silk/mothvm/program.h>

uint32_t checksum(const Program*);

void read_file(const char*, Program*, const char**);

void write_file(const char*, const Program*, const char**);

#ifdef __cplusplus
}
#endif

#endif
