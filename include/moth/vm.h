#ifndef MOTHVM_VM_H
#define MOTHVM_VM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <moth/env.h>
#include <moth/garbage.h>
#include <moth/program.h>
#include <moth/stack.h>

typedef enum {
  STATUS_OK = 0,
  STATUS_INVTYP,
  STATUS_INVARG,
  STATUS_UNDEFN,
  STATUS_NOTFUN,
} VMStatus;

typedef struct {
  Program*         prg;
  uint8_t*         ip;
  Stack            stk;
  VMStatus         st;
  Environment      env;
  GarbageCollector gc;
} VM;

void init_vm(VM*);
void run(VM*, Program*);
void free_vm(VM*);

#ifdef __cplusplus
}
#endif

#endif