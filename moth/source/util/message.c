#include <stdio.h>
#include <stdlib.h>

#include <util/message.h>

void print_err(const char *fmt, ...) {
  va_list vrgs;
  va_start(vrgs, fmt);
  printf(RED BOLD "(!) Error" RESET ": ");
  vprintf(fmt, vrgs);
  printf("\n");
  va_end(vrgs);
}

void print_wrn(const char *fmt, ...) {
  va_list vrgs;
  va_start(vrgs, fmt);
  printf(YELLOW BOLD "(*) Warning" RESET ": ");
  vprintf(fmt, vrgs);
  printf("\n");
  va_end(vrgs);
}