#include <stdint.h>
#include <stdsilk/io.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <moth/ffi.h>
#include <moth/object.h>
#include <moth/value.h>

static const uint32_t FILE_TAG = (STD_SILK_IO_TAG << 16) | 0x0001;

static MOTH_FFI_DELETER_FUN(file_deleter) {
  FILE *file = (FILE *)ptr;
  if (tag == FILE_TAG && file != NULL) fclose((FILE *)file);
}

#define RETURN_CONST_STRING(str)                                               \
  do {                                                                         \
    char *ret = malloc(sizeof(str));                                           \
    strcpy(ret, str);                                                          \
    return ret;                                                                \
  } while (true);

#define RETURN_SPRINTFD_STRING(fmt, val)                                       \
  do {                                                                         \
    char *ret = malloc(sizeof(char) * 50);                                     \
    sprintf(ret, fmt, val);                                                    \
    return ret;                                                                \
  } while (true);

static char *format_value(const Value v) {
  switch (v.type) {
    case T_VOID: RETURN_CONST_STRING("void");
    case T_BOOL: RETURN_CONST_STRING(v.as.boolean ? "true" : "false");
    case T_INT: RETURN_SPRINTFD_STRING("%ld", v.as.integer);
    case T_REAL: RETURN_SPRINTFD_STRING("%lf", v.as.real);
    case T_CHAR: RETURN_SPRINTFD_STRING("%lc", v.as.charac);
    case T_STR: RETURN_CONST_STRING(v.as.string);
    case T_OBJ: RETURN_SPRINTFD_STRING("0x%16lx", (uintptr_t)v.as.object)
  }
}

static char *format_to_string(const char *fmt, Value *argv, uint8_t argc) {
  size_t fmt_len   = sizeof(char) * strlen(fmt);
  size_t fmt_start = 0;

  char * output     = malloc(fmt_len);
  size_t output_len = 0;
  size_t output_cap = fmt_len;

  size_t value_i = 0;

  for (size_t i = 1; i < fmt_len; i++) {
    if (!(fmt[i - 1] == '{' && fmt[i] == '}')) continue;

    size_t copy_amount = i - 1 - fmt_start;

    if (output_cap <= output_len + copy_amount) {
      output_cap = output_len + copy_amount;
      output     = realloc(output, sizeof(char) * output_cap);
    }

    strncpy(output + output_len, fmt + fmt_start, copy_amount);
    output_len += copy_amount;
    fmt_start = i + 1;
    i += 2;

    if (value_i >= argc) {
      free(output);
      return NULL;
    }

    char *formatted_value = format_value(argv[value_i++]);

    if (output_cap <= output_len + strlen(formatted_value)) {
      output_cap = output_len + strlen(formatted_value);
      output     = realloc(output, sizeof(char) * output_cap);
    }

    strcpy(output + output_len, formatted_value);
    output_len += strlen(formatted_value);
    free(formatted_value);
  }

  return output;
}

MOTH_FFI_FUN_BODY(stdsilk_format) {
  MOTH_FFI_FUN_ARITY_MIN(1);
  MOTH_FFI_FUN_ARG_STR(0, format);

  const char *  formatted     = format_to_string(format, argv + 1, argc - 1);
  ObjectString *formatted_str = obj_str_from_raw(formatted);
  free((void *)formatted);

  *ret = OBJ_VAL((Object *)formatted_str);
  return FFI_RESULT_OK;
}

MOTH_FFI_FUN_BODY(stdsilk_print) {
  MOTH_FFI_FUN_ARITY_MIN(1);
  MOTH_FFI_FUN_ARG_STR(0, format);

  const char *formatted = format_to_string(format, argv + 1, argc - 1);

  fputs(formatted, stdout);
  free((void *)formatted);

  return FFI_RESULT_OK;
}

MOTH_FFI_FUN_BODY(stdsilk_scan) {
  MOTH_FFI_FUN_ARITY(0);

  char buf[256];
  if (!fgets(buf, 256, stdin)) return FFI_RESULT_ERROR;

  *ret = OBJ_VAL((Object *)obj_str_from_raw(buf));
  return FFI_RESULT_OK;
}

MOTH_FFI_FUN_BODY(stdsilk_fopen) {
  MOTH_FFI_FUN_ARITY(2);
  MOTH_FFI_FUN_ARG_STR(0, name);
  MOTH_FFI_FUN_ARG_STR(1, mode);

  FILE *file = fopen(name, mode);
  if (!file) return FFI_RESULT_ERROR;
  ObjectFFIPointer *ptr = obj_ffi_ptr_new(FILE_TAG, (void *)file, file_deleter);

  *ret = OBJ_VAL((Object *)ptr);
  return FFI_RESULT_OK;
}

MOTH_FFI_FUN_BODY(stdsilk_fwrite) {
  MOTH_FFI_FUN_ARITY(2);
  MOTH_FFI_FUN_ARG_FFI_PTR(0, fd, FILE *, FILE_TAG);
  MOTH_FFI_FUN_ARG_STR(1, content);

  fputs(content, fd);

  return FFI_RESULT_OK;
}

MOTH_FFI_FUN_BODY(stdsilk_fread) {
  MOTH_FFI_FUN_ARITY(1);
  MOTH_FFI_FUN_ARG_FFI_PTR(0, fd, FILE *, FILE_TAG);

  char buf[256];
  if (!fgets(buf, 256, fd)) return FFI_RESULT_ERROR;

  *ret = OBJ_VAL((Object *)obj_str_from_raw(buf));
  return FFI_RESULT_OK;
}

MOTH_FFI_FUN_BODY(stdsilk_feof) {
  MOTH_FFI_FUN_ARITY(1);
  MOTH_FFI_FUN_ARG_FFI_PTR(0, fd, FILE *, FILE_TAG);

  *ret = BOOL_VAL(feof(fd));
  return FFI_RESULT_OK;
}
