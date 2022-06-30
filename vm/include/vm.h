#ifndef MOTHVM_VM_H
#define MOTHVM_VM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <silk/mothvm/env.h>
#include <silk/mothvm/garbage.h>
#include <silk/mothvm/program.h>
#include <silk/mothvm/stack.h>

typedef enum {
  STATUS_OK = 0,
  STATUS_INVTYP,
  STATUS_INVARG,
  STATUS_INVIDX,
  STATUS_UNDEFN,
  STATUS_NOTFUN,
  STATUS_DEBUGD,
  STATUS_NOPROG,
} VMStatus;

typedef VMStatus (*NativeFn)(size_t, Value*, Value*);

typedef struct {
  Program*         prg;
  bool             dbg;
  uint8_t*         ip;
  Stack            stk;
  VMStatus         sts;
  Environment      env;
  GarbageCollector gc;
} VM;

void init_vm(VM*);
void vm_run(VM*);
void vm_load(VM*, Program*, bool);
void vm_bind(VM*, const char*, NativeFn);
void free_vm(VM*);

#ifdef __cplusplus
}
#endif

#endif
