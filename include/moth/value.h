#ifndef MOTHVM_VALUE_H
#define MOTHVM_VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

typedef struct Object Object;

typedef struct {
  uint32_t hash;
  char *   str;
} Symbol;

typedef enum {
  T_VOID = 2,
  T_BOOL = 3,
  T_INT  = 5,
  T_REAL = 7,
  T_CHAR = 11,
  T_STR  = 13,
  T_OBJ  = 17,
} ValueType;

typedef struct {
  ValueType type;
  union {
    bool    boolean;
    int64_t integer;
    double  real;
    wchar_t charac;
    char *  string;
    Object *object;
  } as;
} Value;

#define IS_VOID(val) val.type == T_VOID
#define IS_BOOL(val) val.type == T_BOOL
#define IS_INT(val)  val.type == T_INT
#define IS_REAL(val) val.type == T_REAL
#define IS_CHAR(val) val.type == T_CHAR
#define IS_STR(val)  val.type == T_STR
#define IS_OBJ(val)  val.type == T_OBJ

#define VOID_VAL                                                               \
  (Value) {                                                                    \
    .type = T_VOID, .as.integer = 0x0                                          \
  }
#define BOOL_VAL(x)                                                            \
  (Value) {                                                                    \
    .type = T_BOOL, .as.boolean = x                                            \
  }
#define INT_VAL(x)                                                             \
  (Value) {                                                                    \
    .type = T_INT, .as.integer = x                                             \
  }
#define REAL_VAL(x)                                                            \
  (Value) {                                                                    \
    .type = T_REAL, .as.real = x                                               \
  }
#define CHAR_VAL(x)                                                            \
  (Value) {                                                                    \
    .type = T_CHAR, .as.charac = x                                             \
  }
#define OBJ_VAL(x)                                                             \
  (Value) {                                                                    \
    .type = T_OBJ, .as.object = x                                              \
  }

bool     truthy(Value);
bool     falsy(Value);
uint32_t hash(const char *);
void     print_value(Value);

#ifdef __cplusplus
}
#endif

#endif