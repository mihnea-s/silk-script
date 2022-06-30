#ifndef MOTHVM_STDLIB_H
#define MOTHVM_STDLIB_H

#include <stdbool.h>
#include <stddef.h>

#include <silk/mothvm/value.h>
#include <silk/mothvm/vm.h>

#ifdef __cplusplus
extern "C" {
#endif

void bind_stdlib(VM*);

#ifdef __cplusplus
}
#endif

#endif
