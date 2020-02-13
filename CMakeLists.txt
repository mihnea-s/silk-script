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
  "src/common/token.cxx"
  "src/common/lexer.cxx"
  "src/common/parser.cxx"
  "src/common/checker.cxx"
  
  # debugger specific
  "src/debugger/debugger.cxx"

  # compiler specific
  "src/compiler/compiler.cxx"

  # repl
  "src/repl/repl.cxx"

  # util
  "src/util/cli.cxx"
  "src/util/parameters.cxx"
  "src/util/filepath.cxx"
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
  CXX_STANDARD 17
)

target_include_directories(${EXECUTABLE_NAME} PUBLIC include vm/include)

target_link_libraries(${EXECUTABLE_NAME} fmt::fmt silkvm)