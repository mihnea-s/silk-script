#ifndef SILKVM_VM_H
#define SILKVM_VM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "program.h"
#include "stack.h"

typedef enum {
  STATUS_OK,
  STATUS_ERR,
  STATUS_FATAL,
} VMStatus;

typedef struct {
  Chunk*   cnk;
  uint8_t* ip;
  Stack    stk;
  VMStatus st;
} VM;

void init_vm(VM*);
void run(VM*, Program*);
void free_vm(VM*);

#ifdef __cplusplus
}
#endif

#endif