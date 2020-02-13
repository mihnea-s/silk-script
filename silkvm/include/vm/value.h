#ifndef SILKVM_VALUE_H
#define SILKVM_VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "object.h"

typedef enum {
  T_VID,  // 0
  T_INT,  // 1
  T_REAL, // 2
  T_BOOL, // 3
  T_STR,  // 4
  T_OBJ,  // 5
  T_REF,  // 6
} ValueType;

typedef struct {
  ValueType type;
  union {
    int64_t integer;
    double  real;
    bool    boolean;
    char*   string;
    Object* object;
  } as;
} Value;

#define IS_VID(val)  val.type == T_VID
#define IS_INT(val)  val.type == T_INT
#define IS_REAL(val) val.type == T_REAL
#define IS_BOOL(val) val.type == T_BOOL
#define IS_STR(val)  val.type == T_STR
#define IS_OBJ(val)  val.type == T_OBJ
#define IS_REF(val)  val.type == T_REF

#define VID_VAL                                                                \
  (Value) {                                                                    \
    .type = T_VID, .as.integer = 0x0                                           \
  }
#define INT_VAL(x)                                                             \
  (Value) {                                                                    \
    .type = T_INT, .as.integer = x                                             \
  }
#define REAL_VAL(x)                                                            \
  (Value) {                                                                    \
    .type = T_REAL, .as.real = x                                               \
  }
#define BOOL_VAL(x)                                                            \
  (Value) {                                                                    \
    .type = T_BOOL, .as.boolean = x                                            \
  }

bool truthy(Value);
bool falsy(Value);

void print_value(Value);

#ifdef __cplusplus
}
#endif

#endif