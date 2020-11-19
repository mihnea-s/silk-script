#ifndef MOTHVM_VALUE_H
#define MOTHVM_VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct Object Object;

typedef struct {
  uint32_t hash;
  char*    str;
} Symbol;

typedef enum {
  T_VID  = 2,
  T_INT  = 3,
  T_REAL = 5,
  T_BOOL = 7,
  T_STR  = 11,
  T_OBJ  = 13,
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
#define OBJ_VAL(x)                                                             \
  (Value) {                                                                    \
    .type = T_OBJ, .as.object = x                                              \
  }

bool     truthy(Value);
bool     falsy(Value);
uint32_t hash(const char*);
void     print_value(Value);

#ifdef __cplusplus
}
#endif

#endif