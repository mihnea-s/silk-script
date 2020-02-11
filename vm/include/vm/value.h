#ifndef SILKVM_VALUE_H
#define SILKVM_VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef int16_t Value;

void print_value(Value);

#ifdef __cplusplus
}
#endif

#endif