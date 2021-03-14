#include <moth/ffi.h>

ObjectFFIFunction *obj_ffi_fun_new(FFIFunction fun) {
  ObjectFFIFunction *obj = (ObjectFFIFunction *)alloc_object(
    O_FFI_FUNCTION, sizeof(ObjectFFIFunction));

  obj->fun = fun;
  return obj;
}

ObjectFFIPointer *obj_ffi_ptr_new(uint32_t tag, void *ptr, FFIDeleter del) {
  ObjectFFIPointer *obj =
    (ObjectFFIPointer *)alloc_object(O_FFI_POINTER, sizeof(ObjectFFIPointer));

  obj->tag = tag;
  obj->ptr = ptr;
  obj->del = del;
  return obj;
}

void obj_ffi_ptr_del(ObjectFFIPointer *ffi_ptr) {
  if (ffi_ptr->del != NULL) ffi_ptr->del(ffi_ptr->tag, ffi_ptr->ptr);
}
