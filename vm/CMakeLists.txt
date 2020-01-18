cmake_minimum_required(VERSION 3.11)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

set(VM_SRC
  source/vm/chunk.c
  source/vm/constants.c
  source/vm/disas.c
  source/vm/mem.c
  source/vm/value.c
  source/vm/vm.c
  source/vm/stack.c
)

project(SilkVM LANGUAGES C)

add_library(silkvm STATIC ${VM_SRC})

add_executable(silkvm-exe
  source/main.c

  source/util/cli.c
  source/util/read_file.c
  source/util/message.c
)

target_include_directories(silkvm PUBLIC include/vm)
target_include_directories(silkvm-exe PRIVATE include/)

target_link_libraries(silkvm-exe silkvm)
