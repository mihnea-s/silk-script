#include <stdio.h>

#include <value.h>

void print_value(Value v) {
  if (IS_VID(v)) {
    printf("[vid]");
  } else if (IS_INT(v)) {
    printf("%d", v.as.integer);
  } else if (IS_REAL(v)) {
    printf("%lf", v.as.real);
  } else if (IS_BOOL(v)) {
    printf("%s", v.as.boolean ? "true" : "false");
  } else if (IS_STR(v)) {
    printf("'%s'", v.as.string);
  }
}
