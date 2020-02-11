cmake_minimum_required(VERSION 3.1)
project(SilkTool C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(EXECUTABLE_NAME           "silk")

# silkvm
add_subdirectory(vm)

# {fmt}
add_subdirectory(vendor/fmt-6.1.2)

add_executable(${EXECUTABLE_NAME}
  "src/main.cxx"

  # common files
  "src/common/error.cxx"
  "src/common/lexer.cxx"
  "src/common/token.cxx"
  
  # interpreter specific
  "src/interpreter/repl.cxx"
  "src/interpreter/analyzers/checker.cxx"
  "src/interpreter/analyzers/parser.cxx"
  "src/interpreter/runtime/interpreter.cxx"
  "src/interpreter/runtime/stdlib.cxx"
  "src/interpreter/runtime/functions.cxx"
  "src/interpreter/runtime/primitives.cxx"
  "src/interpreter/runtime/structs.cxx"

  # compiler specific
  "src/compiler/compiler.cxx"

  # util
  "src/util/parameters.cxx"
  "src/util/cli.cxx"
  "src/util/filepath.cxx"
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
  CXX_STANDARD 17
)

target_include_directories(${EXECUTABLE_NAME} PUBLIC include vm/include)

target_link_libraries(${EXECUTABLE_NAME} fmt::fmt silkvm)