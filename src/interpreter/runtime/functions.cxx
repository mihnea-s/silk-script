#include <cstdint>
#include <string>

#include <silk/interpreter/ast/stmt.h>
#include <silk/interpreter/runtime/functions.h>
#include <silk/interpreter/runtime/interpreter.h>
#include <silk/interpreter/runtime/primitives.h>

// Function ---------------------------

std::size_t Function::arity() const {
  return _args.size();
}

void Function::call(std::vector<ObjectPtr>& args) {
  auto scp = _env.from_scope(_closure);

  for (auto i = 0; i < args.size(); i++) {
    _env.define(_args[i].first, args[i]);
  }

  _interp.execute_stmt(_body);
}

std::string Function::string() {
  return "[fct]";
}

bool Function::truthy() {
  return true;
}

ObjectPtr Function::operator==(ObjectPtr&) {
  return obj::make(true);
}

// NativeFunction ---------------------

std::size_t NativeFunction::arity() const {
  return _args;
}

void NativeFunction::call(std::vector<ObjectPtr>& args) {
  throw Interpreter::Interrupt {
    _native(args),
    Stmt::Interrupt::Type::ret,
  };
}

std::string NativeFunction::string() {
  return "[native fct]";
}

bool NativeFunction::truthy() {
  return true;
}

ObjectPtr NativeFunction::operator==(ObjectPtr&) {
  return obj::make(true);
}