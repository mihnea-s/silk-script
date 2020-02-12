#include <stdio.h>

#include <value.h>

#define PRINT_BR(...)                                                          \
  printf(__VA_ARGS__);                                                         \
  break;

void print_value(Value v) {
  switch (v.type) {
    case T_INT: PRINT_BR("%d", v.as.integer);
    case T_REAL: PRINT_BR("%lf", v.as.real);
    case T_BOOL: PRINT_BR("%s", v.as.boolean ? "true" : "false");
    case T_STR: PRINT_BR("'%s'", v.as.string);
    case T_VID: PRINT_BR("[vid]");
  }
}
