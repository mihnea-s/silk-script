cmake_minimum_required(VERSION 3.15)

include(FetchContent)

project(
  SilkTool
  
  VERSION 0.0.1
  DESCRIPTION "Silk compiler and virtual machine"
  LANGUAGES C CXX
)

set(SILK_COMPILER       "silk")
set(SILK_VIRTUALMACHINE "moth")
set(SILK_STDLIBRARY     "stdsilk")
set(SILK_DOCUMENTATION  "docs")

set(CMAKE_EXPORT_COMPILE_COMMANDS on CACHE INTERNAL "")

if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

# Add Sanitize profile to multiconfig generators
get_property(CMAKE_GENERATOR_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (CMAKE_GENERATOR_IS_MULTI_CONFIG AND NOT "Sanitize" IN_LIST CMAKE_CONFIGURATION_TYPES)
  list(APPEND CMAKE_CONFIGURATION_TYPES "Sanitize")
endif()

set(SANITIZE_LINKER_FLAGS "-fsanitize=address")
set(SANITIZE_COMPILER_FLAGS "${SANITIZE_LINKER_FLAGS} -fno-omit-frame-pointer")

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_C_FLAGS_SANITIZE             "${CMAKE_C_FLAGS_DEBUG} ${SANITIZE_COMPILER_FLAGS}"          )
set(CMAKE_CXX_FLAGS_SANITIZE           "${CMAKE_CXX_FLAG_DEBUG} ${SANITIZE_COMPILER_FLAGS}"         )
set(CMAKE_EXE_LINKER_FLAGS_SANITIZE    "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${SANITIZE_LINKER_FLAGS}"   )

FetchContent_Declare(
  Catch2
  GIT_TAG        "v2.13.1"
  GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
)

FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(
  fmtlib
  GIT_TAG        "5.3.0"
  GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
)
  
FetchContent_MakeAvailable(fmtlib)

add_library(${SILK_VIRTUALMACHINE} STATIC
  "source/moth/mem.c"

  "source/moth/env.c"
  "source/moth/stack.c"

  "source/moth/rodata.c"
  "source/moth/symtable.c"
  "source/moth/program.c"

  "source/moth/file.c"
  "source/moth/disas.c"

  "source/moth/ffi.c"
  "source/moth/value.c"
  "source/moth/object.c"

  "source/moth/garbage.c"
  "source/moth/vm.c"
)

if (NOT WIN32)
  set(C_MATH_LIB "m")
endif()

target_include_directories(${SILK_VIRTUALMACHINE} PUBLIC "include")

target_link_libraries(${SILK_VIRTUALMACHINE} ${C_MATH_LIB})

add_library(${SILK_STDLIBRARY} SHARED
  "source/stdsilk/io.c"
)

set_target_properties(${SILK_STDLIBRARY} PROPERTIES VERSION ${PROJECT_VERSION})

target_include_directories(${SILK_STDLIBRARY} PUBLIC "include")

target_link_libraries(${SILK_STDLIBRARY} ${SILK_VIRTUALMACHINE} ${C_MATH_LIB})

add_executable(${SILK_COMPILER}
  "source/silk/main.cxx"
  "source/silk/utility/cli.cxx"
  
  "source/silk/tools/debugger.cxx"
  "source/silk/tools/repl.cxx"

  "source/silk/language/package.cxx"
  "source/silk/language/scanner.cxx"
  
  "source/silk/pipeline/stage.cxx"
  "source/silk/pipeline/parser.cxx"
  "source/silk/pipeline/type_checker.cxx"
  "source/silk/pipeline/optimizer.cxx"
  "source/silk/pipeline/json_serializer.cxx"
  "source/silk/pipeline/json_deserializer.cxx"
  "source/silk/pipeline/context_builder.cxx"

  "source/silk/targets/js/transpiler.cxx"
  "source/silk/targets/moth/compiler.cxx"
  "source/silk/targets/wasm/compiler.cxx"
)

set_target_properties(${SILK_COMPILER} PROPERTIES
  CXX_STANDARD 17
)

target_include_directories(${SILK_COMPILER} PUBLIC "include")

target_link_libraries(${SILK_COMPILER} ${SILK_VIRTUALMACHINE} fmt::fmt)
