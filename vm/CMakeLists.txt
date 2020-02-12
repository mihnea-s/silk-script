cmake_minimum_required(VERSION 3.11)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

set(SILK_VM_LIB "silkvm")
set(SILK_VM_EXE "sce-run")

set(VM_SRC
  "source/vm/chunk.c"
  "source/vm/constants.c"
  "source/vm/disas.c"
  "source/vm/mem.c"
  "source/vm/program.c"
  "source/vm/value.c"
  "source/vm/vm.c"
  "source/vm/stack.c"
  "source/vm/file_exec.c"
)

project(SilkVM LANGUAGES C)

add_library(${SILK_VM_LIB} STATIC ${VM_SRC})

add_executable(${SILK_VM_EXE}
  "source/main.c"

  "source/util/cli.c"
  "source/util/message.c"
)

target_include_directories(${SILK_VM_LIB} PUBLIC include/vm)
target_include_directories(${SILK_VM_EXE} PRIVATE include/)

target_link_libraries(${SILK_VM_EXE} ${SILK_VM_LIB})
