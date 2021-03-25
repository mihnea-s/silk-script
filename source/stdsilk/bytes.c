#include <stdsilk/bytes.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <moth/value.h>
#include <moth/ffi.h>

static MOTH_FFI_DELETER_FUN(bytes_deleter) {
  if (tag == STD_SILK_BYTES_TAG && ptr != NULL) free(ptr);
}

MOTH_FFI_FUN_BODY(stdsilk_bytes_alloc) {
  MOTH_FFI_FUN_ARITY(1);
  MOTH_FFI_FUN_ARG(0, size, IS_INT);

  void *bytes        = malloc(size.as.integer + sizeof(uint64_t));
  *(uint64_t *)bytes = size.as.integer;

  *ret = OBJ_VAL(
    (Object *)obj_ffi_ptr_new(STD_SILK_BYTES_TAG, bytes, bytes_deleter));
  return FFI_RESULT_OK;
}

MOTH_FFI_FUN_BODY(stdsilk_bytes_open) {
  MOTH_FFI_FUN_ARITY(2);
  MOTH_FFI_FUN_ARG_STR(0, name);
  MOTH_FFI_FUN_ARG_STR(1, mode);

  FILE* file = fopen(name, mode);
  if (!file) return FFI_RESULT_ERROR;
  
  uint64_t begin = ftell(file);
  fseek(file, 0, SEEK_END);

  uint64_t size = ftell(file) - begin;
  fseek(file, 0, SEEK_SET);

  void *bytes        = malloc(size + sizeof(uint64_t));
  *(uint64_t *)bytes = size;

  fread(bytes + sizeof(uint64_t), size, 0x1, file);
  fclose(file);

  *ret = OBJ_VAL(
    (Object *)obj_ffi_ptr_new(STD_SILK_BYTES_TAG, bytes, bytes_deleter));
  return FFI_RESULT_OK;
}

MOTH_FFI_FUN_BODY(stdsilk_bytes_read) {
}

MOTH_FFI_FUN_BODY(stdsilk_bytes_write) {
}

MOTH_FFI_FUN_BODY(stdsilk_bytes_to_array) {
}

MOTH_FFI_FUN_BODY(stdsilk_bytes_to_string) {
}
