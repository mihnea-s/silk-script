#ifndef STDSILK_IO_H
#define STDSILK_IO_H

#include <moth/ffi.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STD_SILK_IO_TAG 0x2000

MOTH_FFI_FUN_DEF(stdsilk_format);

MOTH_FFI_FUN_DEF(stdsilk_print);

MOTH_FFI_FUN_DEF(stdsilk_scan);

MOTH_FFI_FUN_DEF(stdsilk_fopen);

MOTH_FFI_FUN_DEF(stdsilk_fwrite);

MOTH_FFI_FUN_DEF(stdsilk_fread);

MOTH_FFI_FUN_DEF(stdsilk_feof);

#ifdef __cplusplus
}
#endif

#endif
