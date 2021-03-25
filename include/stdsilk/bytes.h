#ifndef STDSILK_BYTES_H
#define STDSILK_BYTES_H

#include <moth/ffi.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STD_SILK_BYTES_TAG 0x2001

MOTH_FFI_FUN_DEF(stdsilk_bytes_alloc);

MOTH_FFI_FUN_DEF(stdsilk_bytes_open);

MOTH_FFI_FUN_DEF(stdsilk_bytes_read);

MOTH_FFI_FUN_DEF(stdsilk_bytes_write);

MOTH_FFI_FUN_DEF(stdsilk_bytes_to_array);

MOTH_FFI_FUN_DEF(stdsilk_bytes_to_string);

#ifdef __cplusplus
}
#endif

#endif
