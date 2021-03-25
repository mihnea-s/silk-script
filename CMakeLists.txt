cmake_minimum_required(VERSION 3.15)

include(FetchContent)

project(
  SilkTool
  
  VERSION 0.0.1
  DESCRIPTION "Silk compiler and virtual machine"
  LANGUAGES C CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

set(SILK_COMPILER       "silk")
set(SILK_VIRTUALMACHINE "moth")
set(SILK_STDLIBRARY     "stdsilk")
set(SILK_DOCUMENTATION  "docs")

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

  "source/silk/tools/cli.cxx"
  "source/silk/tools/debugger.cxx"
  "source/silk/tools/repl.cxx"
  "source/silk/tools/stprinter.cxx"

  "source/silk/syntax/package.cxx"
  "source/silk/syntax/scanner.cxx"
  
  "source/silk/stages/pipeline.cxx"
  "source/silk/stages/parser.cxx"
  "source/silk/stages/type_checker.cxx"
  "source/silk/stages/optimizer.cxx"

  "source/silk/compiler/mothvm.cxx"
  "source/silk/compiler/webasm.cxx"
)

set_target_properties(${SILK_COMPILER} PROPERTIES
  CXX_STANDARD 17
)

target_include_directories(${SILK_COMPILER} PUBLIC "include")

target_link_libraries(${SILK_COMPILER} ${SILK_VIRTUALMACHINE} fmt::fmt)

find_program(STANDARDESE_TOOL standardese)

if(NOT STANDARDESE_TOOL)
  message("-- Standardese not found, skipping documentation")
else()
  message("-- Standardese found, will build documentation")

  add_custom_target(
    ${SILK_DOCUMENTATION}
    COMMAND ${STANDARDESE_TOOL}
      --config ${CMAKE_CURRENT_SOURCE_DIR}/standardese.cfg
      --output.prefix=${CMAKE_CURRENT_BINARY_DIR}/documentation/
      "include/silk/parser/"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating documentation using standardese..."
    VERBATIM
  )
endif()
