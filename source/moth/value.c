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
