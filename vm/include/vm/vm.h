#ifndef SILKVM_VM_H
#define SILKVM_VM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "chunk.h"
#include "stack.h"

typedef enum {
  VM_OK,
  VM_ERR,
} VMStatus;

typedef struct {
  Chunk*   cnk;
  uint8_t* ip;
  Stack    stk;
  VMStatus st;
} VM;

void init_vm(VM*);
void run(VM*, Chunk*);
void free_vm(VM*);

#ifdef __cplusplus
}
#endif

#endif