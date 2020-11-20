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

add_executable(${SILK_COMPILER}
  "source/silk/main.cxx"

  "source/silk/lexer/token.cxx"
  "source/silk/lexer/lexer.cxx"
  
  "source/silk/parser/ast.cxx"
  "source/silk/parser/parser.cxx"
  
  "source/silk/compiler/compiler.cxx"
  "source/silk/compiler/type_checker.cxx"

  "source/silk/tools/debugger.cxx"
  "source/silk/tools/repl.cxx"

  "source/silk/util/cli.cxx"
  "source/silk/util/error.cxx"
)

set_target_properties(${SILK_COMPILER} PROPERTIES
  CXX_STANDARD 17
)

target_include_directories(${SILK_COMPILER} PUBLIC "include")

target_link_libraries(${SILK_COMPILER} ${SILK_VIRTUALMACHINE} fmt::fmt)
