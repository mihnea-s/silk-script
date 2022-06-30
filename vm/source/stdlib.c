#include <stdio.h>

#include <silk/mothvm/stdlib.h>

#include <silk/mothvm/value.h>
#include <silk/mothvm/vm.h>

static VMStatus stdlib_print(size_t argc, Value* argv, Value* ret) {
  for (Value* value = argv; value < argv + argc; value++) {
    print_value(*value);
    putc(' ', stdout);
  }

  putc('\n', stdout);

  return STATUS_OK;
}

void bind_stdlib(VM* vm) {
  vm_bind(vm, "print", stdlib_print);
}
