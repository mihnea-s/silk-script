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

static void file_deleter(uint32_t tag, void *file) {
  if (tag == FILE_TAG && file != NULL) fclose((FILE *)file);
}

#define RETURN_CONST_STRING(str)                                               \
  do {                                                                         \
    char *ret = malloc(sizeof(char) * strlen(str));                            \
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

MOTH_FFI_PUB(stdsilk_format) {
  if (argc < 1) return FFI_RESULT_ARITY;
  if (!IS_STR(argv[0])) return FFI_RESULT_TYPES;

  const char *formatted = format_to_string(argv->as.string, argv + 1, argc - 1);
  *ret                  = OBJ_VAL((Object *)obj_str_from_raw(formatted));

  free((void *)formatted);
  return FFI_RESULT_OK;
}

MOTH_FFI_PUB(stdsilk_print) {
  if (argc < 1) return FFI_RESULT_ARITY;
  if (!IS_STR(argv[0])) return FFI_RESULT_TYPES;

  const char *formatted = format_to_string(argv->as.string, argv + 1, argc - 1);
  fputs(formatted, stdout);
  free((void *)formatted);

  return FFI_RESULT_OK;
}

MOTH_FFI_PUB(stdsilk_scan) {
  if (argc < 1) return FFI_RESULT_ARITY;
  if (!IS_STR(argv[0])) return FFI_RESULT_TYPES;

  char buf[256];
  if (fgets(buf, 256, stdin) == NULL) return FFI_RESULT_ERROR;

  *ret = OBJ_VAL((Object *)obj_str_from_raw(buf));
  return FFI_RESULT_OK;
}

MOTH_FFI_PRIV(stdsilk_file_write) {
  MOTH_FFI_METHOD_CHECK(1);

  ObjectDictionary *this = MOTH_FFI_METHOD_THIS();
  MOTH_FFI_METHOD_MEMBER(this, fd, IS_OBJ_FFI_PTR);

  FILE *file = (FILE *)OBJ_FFI_PTR(fd.as.object)->ptr;

  const char *content = string_value(argv[1]);
  if (!content) return FFI_RESULT_ERROR;
  fputs(content, file);

  return FFI_RESULT_OK;
}

MOTH_FFI_PRIV(stdsilk_file_read) {
  MOTH_FFI_METHOD_CHECK(0);

  ObjectDictionary *this = MOTH_FFI_METHOD_THIS();
  MOTH_FFI_METHOD_MEMBER(this, fd, IS_OBJ_FFI_PTR);

  FILE *file = (FILE *)OBJ_FFI_PTR(fd.as.object)->ptr;

  char buf[256];
  if (fgets(buf, 256, file) == NULL) return FFI_RESULT_ERROR;

  *ret = OBJ_VAL((Object *)obj_str_from_raw(buf));
  return FFI_RESULT_OK;
}

MOTH_FFI_PUB(stdsilk_open) {
  if (argc < 2) return FFI_RESULT_ARITY;

  const char *name = string_value(argv[0]), *mode = string_value(argv[1]);

  if (!name || !mode) return FFI_RESULT_TYPES;

  FILE *file = fopen(name, mode);

  Value keys[] = {
    OBJ_VAL((Object *)obj_str_from_raw("fd")),
    OBJ_VAL((Object *)obj_str_from_raw("write")),
    OBJ_VAL((Object *)obj_str_from_raw("read")),
  };

  Value vals[] = {
    OBJ_VAL((Object *)obj_ffi_ptr_new(FILE_TAG, (void *)file, file_deleter)),
    OBJ_VAL((Object *)obj_ffi_fun_new(stdsilk_file_write)),
    OBJ_VAL((Object *)obj_ffi_fun_new(stdsilk_file_read)),
  };

  size_t len = sizeof(keys) / sizeof(Value);

  *ret = OBJ_VAL((Object *)obj_dct_from_raw(keys, vals, len));
  return FFI_RESULT_OK;
}
