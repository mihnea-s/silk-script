#ifndef MOTHVM_VALUE_H
#define MOTHVM_VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct Object Object;

typedef uint8_t byte_t;

typedef bool bool_t;

typedef uint64_t natural_t;

typedef int64_t integer_t;

typedef double real_t;

typedef double* vector_t;

typedef uint32_t rune_t;

typedef char* string_t;

typedef void* native_t;

typedef struct {
  uint32_t hash;
  char*    str;
} Symbol;

typedef enum {
  T_BYTE    = 2,
  T_BOOL    = 3,
  T_NATURAL = 5,
  T_INTEGER = 7,
  T_REAL    = 11,
  T_RUNE    = 13,
  T_STRING  = 17,
  T_OBJECT  = 19,
  T_NATIVE  = 23,
} ValueType;

typedef struct {
  ValueType type;
  union {
    byte_t    byte;
    bool_t    boolean;
    natural_t natural;
    integer_t integer;
    real_t    real;
    rune_t    rune;
    string_t  string;
    Object*   object;
    native_t  native;
  } as;
} Value;

bool truthy(Value v);
bool falsy(Value v);

uint32_t hash(const char* str);
uint32_t hash_value(Value v);

const char* string_value(Value v);
bool        equal_values(Value a, Value b);
void        print_value(Value v);

#ifdef __cplusplus
}
#endif

#endif
