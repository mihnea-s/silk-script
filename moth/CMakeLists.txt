cmake_minimum_required(VERSION 3.11)

project(MothVM LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

set(MOTH_VM_LIB "mothvm")
set(MOTH_VM_EXE   "moth")

if (NOT WIN32)
  set(C_MATH_LIB "m")
endif()

set(VM_SRC
  "source/vm/mem.c"

  "source/vm/stack.c"
  "source/vm/hash_map.c"

  "source/vm/rodata.c"
  "source/vm/symtable.c"
  "source/vm/program.c"
  
  "source/vm/file.c"
  "source/vm/disas.c"
  
  "source/vm/value.c"
  "source/vm/object.c"
  "source/vm/garbage.c"
  "source/vm/vm.c"
)

add_library(${MOTH_VM_LIB} STATIC ${VM_SRC})

add_executable(${MOTH_VM_EXE}
  "source/main.c"

  "source/util/cli.c"
  "source/util/message.c"
)

target_include_directories(${MOTH_VM_LIB} PUBLIC include/vm)
target_include_directories(${MOTH_VM_EXE} PRIVATE include/)

target_link_libraries(${MOTH_VM_LIB} ${C_MATH_LIB})
target_link_libraries(${MOTH_VM_EXE} ${MOTH_VM_LIB})
