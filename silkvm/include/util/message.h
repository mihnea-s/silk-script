#ifndef SILKVMEXE_MESSAGE_H
#define SILKVMEXE_MESSAGE_H

#include <stdarg.h>
#include <stdio.h>

#define RESET "\033[00m"
#define BOLD "\033[1m"
#define DARK "\033[2m"
#define UNDERLINE "\033[4m"
#define BLINK "\033[5m"
#define REVERSE "\033[7m"
#define CONCEALED "\033[8m"
#define GRAY "\033[30m"
#define GREY "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define BG_GRAY "\033[40m"
#define BG_GREY "\033[40m"
#define BG_RED "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_YELLOW "\033[43m"
#define BG_BLUE "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN "\033[46m"
#define BG_WHITE "\033[47m"

void print_err(const char *fmt, ...);
void print_wrn(const char *fmt, ...);

#endif