#include <stdio.h>

#include <value.h>

#define PRINT_BR(...)                                                          \
  printf(__VA_ARGS__);                                                         \
  break;

bool truthy(Value v) {
  switch (v.type) {
    case T_VID: return false;
    case T_INT: return v.as.integer != 0;
    case T_REAL: return v.as.real != 0.0;
    case T_BOOL: return v.as.boolean;
    case T_STR: return v.as.string[0] != '\0';
    case T_OBJ: return true;
  }
}

bool falsy(Value v) {
  return !truthy(v);
}

void print_value(Value v) {
  switch (v.type) {
    case T_INT: PRINT_BR("%ld", v.as.integer);
    case T_REAL: PRINT_BR("%lf", v.as.real);
    case T_BOOL: PRINT_BR("%s", v.as.boolean ? "true" : "false");
    case T_STR: PRINT_BR("'%s'", v.as.string);
    case T_VID: PRINT_BR("[vid]");
    case T_OBJ: PRINT_BR("[object]")
  }
}
