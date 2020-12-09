#ifndef MOTH_FILE_H
#define MOTH_FILE_H

#include <bits/stdint-uintn.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <moth/program.h>

uint32_t checksum(Program*);

void read_file(const char*, Program*, const char**);

void write_file(const char*, Program*, const char**);

#ifdef __cplusplus
}
#endif

#endif