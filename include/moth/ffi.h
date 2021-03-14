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

#define MOTH_FFI_PUB(fn)                                                       \
  MOTH_FFI_API FFIResult fn(Value *argv, uint8_t argc, Value *ret)

#define MOTH_FFI_PRIV(fn)                                                      \
  static FFIResult fn(Value *argv, uint8_t argc, Value *ret)

#define MOTH_FFI_METHOD_CHECK(arity)                                           \
  do {                                                                         \
    if (argc < arity + 1) return FFI_RESULT_ARITY;                             \
    if (!IS_OBJ_DCT(argv[0])) return FFI_RESULT_TYPES;                         \
  } while (false)

#define MOTH_FFI_METHOD_THIS() OBJ_DCT(argv[0].as.object)

#define MOTH_FFI_METHOD_MEMBER(it, member, check)                              \
  Value member =                                                               \
    obj_dct_get(it, OBJ_VAL((Object *)obj_str_from_raw(#member)));             \
  if (!check(member)) return FFI_RESULT_ERROR;

typedef enum {
  FFI_RESULT_OK = 0,
  FFI_RESULT_ERROR,
  FFI_RESULT_ARITY,
  FFI_RESULT_TYPES,
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
