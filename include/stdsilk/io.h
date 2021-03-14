#ifndef STDSILK_IO_H
#define STDSILK_IO_H

#include <moth/ffi.h>

#define STD_SILK_IO_TAG 0x2000

/**
 *  Format values into a string.
 */
MOTH_FFI_PUB(stdsilk_format);

/**
 *  Print function.
 */
MOTH_FFI_PUB(stdsilk_print);

/**
 *  Scan function.
 */
MOTH_FFI_PUB(stdsilk_scan);

/**
 *  Open a file function
 */
MOTH_FFI_PUB(stdsilk_open);

#endif
