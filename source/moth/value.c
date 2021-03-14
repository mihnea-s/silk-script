#include <moth/value.h>

#include <stdio.h>
#include <string.h>

#include <moth/object.h>

#define PRINT_BR(...)                                                          \
  printf(__VA_ARGS__);                                                         \
  break;

bool truthy(Value v) {
  switch (v.type) {
    case T_VOID: return false;
    case T_BOOL: return v.as.boolean;
    case T_INT: return v.as.integer != 0;
    case T_REAL: return v.as.real != 0.0;
    case T_CHAR: return v.as.charac != '\0';
    case T_STR: return v.as.string[0] != '\0';
    case T_OBJ: return true;
  }
}

bool falsy(Value v) {
  return !truthy(v);
}

uint32_t hash(const char *str) {
  uint32_t x = 2166136261u;

  for (size_t i = 0; i < strlen(str); i++) {
    x *= 16777619u;
    x ^= str[i];
  }

  return x;
}

uint32_t hash_value(const Value v) {
  uint32_t x = 2166136261u;

  switch (v.type) {
    case T_VOID: return 0;
    case T_STR: return hash(v.as.string);

    case T_BOOL:
      x *= 1046527u;
      x ^= v.as.boolean * 121021u;
      break;

    case T_INT:
      x *= 16769023u;
      x ^= v.as.integer * 151121u;
      break;

    case T_REAL:
      x *= 112909u;
      x ^= (uint32_t)(v.as.real * 180181u);
      break;

    case T_CHAR:
      x *= 479001599u;
      x ^= v.as.charac;
      break;

    case T_OBJ:
      x *= 16777619u;
      x ^= (uintptr_t)v.as.object;
      break;
  }

  return x;
}

const char *string_value(Value v) {
  if (IS_STR(v)) return v.as.string;
  if (IS_OBJ_STR(v)) return OBJ_STR(v.as.object)->data;
  return NULL;
}

bool equal_values(Value a, Value b) {
  if (a.type != b.type) return false;
  switch (a.type) {
    case T_VOID: return true;
    case T_BOOL: return a.as.boolean == b.as.boolean;
    case T_INT: return a.as.integer == b.as.integer;
    case T_REAL: return a.as.real == b.as.real;
    case T_CHAR: return a.as.charac == b.as.charac;
    case T_STR: return a.as.string == b.as.string;
    case T_OBJ: return equal_objects(a.as.object, b.as.object);
  }
}

void print_value(Value v) {
  switch (v.type) {
    case T_VOID: PRINT_BR("{void}");
    case T_BOOL: PRINT_BR("%s", v.as.boolean ? "true" : "false");
    case T_INT: PRINT_BR("%ld", v.as.integer);
    case T_REAL: PRINT_BR("%lf", v.as.real);
    case T_CHAR: PRINT_BR("\"%lc", v.as.charac);
    case T_STR: PRINT_BR("'%s'", v.as.string);
    case T_OBJ:
      printf("obj = ");
      print_object(v.as.object);
      break;
  }
}
