#ifndef MOTHVM_SYMDATA_H
#define MOTHVM_SYMDATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include <moth/value.h>

typedef struct {
  uint32_t len;
  uint32_t cap;
  Symbol*  arr;
} Symtable;

void init_symtable(Symtable*, uint32_t);
void symtable_write(Symtable*, Symbol);
void free_symtable(Symtable*);

#ifdef __cplusplus
}
#endif

#endif