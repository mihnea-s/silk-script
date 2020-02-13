cmake_minimum_required(VERSION 3.1)
project(SilkTool C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(EXECUTABLE_NAME           "silk")
set(SILKVM_FOLDER           "silkvm")

# silkvm
add_subdirectory(${SILKVM_FOLDER})

# {fmt}
add_subdirectory(vendor/fmt-6.1.2)

add_executable(${EXECUTABLE_NAME}
  "source/main.cxx"

  # common files
  "source/common/error.cxx"
  "source/common/token.cxx"
  "source/common/lexer.cxx"
  "source/common/parser.cxx"
  "source/common/checker.cxx"
  
  # debugger specific
  "source/debugger/debugger.cxx"

  # compiler specific
  "source/compiler/compiler.cxx"

  # repl
  "source/repl/repl.cxx"

  # util
  "source/util/cli.cxx"
  "source/util/parameters.cxx"
  "source/util/filepath.cxx"
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
  CXX_STANDARD 17
)

target_include_directories(${EXECUTABLE_NAME} PUBLIC include ${SILKVM_FOLDER}/include)

target_link_libraries(${EXECUTABLE_NAME} fmt::fmt silkvm)