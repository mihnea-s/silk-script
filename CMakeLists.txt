cmake_minimum_required(VERSION 3.1)
project(SilkInterpreter)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(EXECUTABLE_NAME silk)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(vendor/fmt-6.1.2)

add_executable(${EXECUTABLE_NAME} 
  src/main.cxx
  src/error.cxx
  src/repl.cxx
  src/lexer.cxx

  src/analyzers/checker.cxx
  src/analyzers/parser.cxx

  src/ast/token.cxx

  src/runtime/interpreter.cxx
  src/runtime/stdlib.cxx
  src/runtime/functions.cxx
  src/runtime/primitives.cxx
  src/runtime/structs.cxx
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
  CXX_STANDARD 17
)

target_include_directories(${EXECUTABLE_NAME} PUBLIC include)

target_link_libraries(${EXECUTABLE_NAME} ${CONAN_LIBS} fmt::fmt)
