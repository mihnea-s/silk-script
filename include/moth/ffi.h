#ifndef MOTH_FFI_H
#define MOTH_FFI_H

#include <stdint.h>

#include <moth/object.h>
#include <moth/value.h>

#ifdef _WIN32
  #define MOTH_FFI_API __declspec(dllexport)
#else
  #define MOTH_FFI_API
#endif

#define MOTH_FFI_FUN_DEF(FN_NAME)                                              \
  MOTH_FFI_API FFIResult FN_NAME(Value *argv, uint8_t argc, Value *ret)

#define MOTH_FFI_FUN_BODY MOTH_FFI_FUN_DEF

#define MOTH_FFI_DELETER_FUN(FN_NAME) void FN_NAME(uint32_t tag, void *ptr)

#define MOTH_FFI_FUN_ARITY(ARITY)                                              \
  do {                                                                         \
    if (argc != ARITY) return FFI_RESULT_ARITY;                                \
  } while (false)

#define MOTH_FFI_FUN_ARITY_MIN(ARITY)                                          \
  do {                                                                         \
    if (argc < ARITY) return FFI_RESULT_ARITY;                                 \
  } while (false)

#define MOTH_FFI_FUN_ARG(IDX, ARGNAME, VALIDITY_CHECK)                         \
  Value ARGNAME = argv[IDX];                                                   \
  if (!VALIDITY_CHECK(ARGNAME)) return FFI_RESULT_TYPES;

#define MOTH_FFI_FUN_ARG_STR(IDX, ARGNAME)                                     \
  const char *ARGNAME = string_value(argv[IDX]);                               \
  if (!ARGNAME) return FFI_RESULT_TYPES;

#define MOTH_FFI_FUN_ARG_FFI_PTR(IDX, ARGNAME, TPTR, TAG)                      \
  if (!IS_OBJ_FFI_PTR(argv[IDX])) return FFI_RESULT_TYPES;                     \
  if (OBJ_FFI_PTR(argv[IDX].as.object)->tag != TAG) return FFI_RESULT_TAG;     \
  TPTR ARGNAME = (TPTR)OBJ_FFI_PTR(argv[IDX].as.object)->ptr;

typedef enum {
  FFI_RESULT_OK = 0,
  FFI_RESULT_ERROR,
  FFI_RESULT_ARITY,
  FFI_RESULT_TYPES,
  FFI_RESULT_TAG,
} FFIResult;

typedef FFIResult (*FFIFunction)(Value *, uint8_t, Value *);

typedef struct {
  Object      obj;
  FFIFunction fun;
} ObjectFFIFunction;

typedef void (*FFIDeleter)(uint32_t, void *);

typedef struct {
  Object     obj;
  uint32_t   tag;
  void *     ptr;
  FFIDeleter del;
} ObjectFFIPointer;

#define IS_OBJ_FFI_FCT(val)                                                    \
  (IS_OBJ(val) && val.as.object->type == O_FFI_FUNCTION)
#define IS_OBJ_FFI_PTR(val)                                                    \
  (IS_OBJ(val) && val.as.object->type == O_FFI_POINTER)

#define OBJ_FFI_FUN(obj) ((ObjectFFIFunction *)obj)
#define OBJ_FFI_PTR(obj) ((ObjectFFIPointer *)obj)

ObjectFFIFunction *obj_ffi_fun_new(FFIFunction fun);
ObjectFFIPointer * obj_ffi_ptr_new(uint32_t tag, void *ptr, FFIDeleter del);
void               obj_ffi_ptr_del(ObjectFFIPointer *ffi_ptr);

#endif
