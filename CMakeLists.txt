cmake_minimum_required(VERSION 3.1)
project(SilkTool C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

set(COMPILER       "silk")
set(VIRTUALMACHINE "moth")

# {fmt}
add_subdirectory(vendor/fmt-6.1.2)

add_library(${VIRTUALMACHINE} STATIC
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

target_include_directories(${VIRTUALMACHINE} PUBLIC "include")

target_link_libraries(${VIRTUALMACHINE} ${C_MATH_LIB})

add_executable(${COMPILER}
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

set_target_properties(${COMPILER} PROPERTIES
  CXX_STANDARD 17
)

target_include_directories(${COMPILER} PUBLIC "include")

target_link_libraries(${COMPILER} fmt::fmt ${VIRTUALMACHINE})
