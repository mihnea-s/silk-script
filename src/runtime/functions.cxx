#include <cstdint>
#include <string>

#include <silk/runtime/functions.h>
#include <silk/runtime/interpreter.h>
#include <silk/runtime/interupt.h>
#include <silk/runtime/objects.h>

// Function ---------------------------

std::size_t Function::arity() const {
  return _args.size();
}

std::string Function::argument(std::size_t index) {
  return _args.at(index);
}

void Function::call(Environment<ObjectPtr>& env) {
  _intr.execute_stmt(_body);
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
  return _args.size();
}

std::string NativeFunction::argument(std::size_t index) {
  return _args.at(index);
}

void NativeFunction::call(Environment<ObjectPtr>& env) {
  throw Interupt {
    Interupt::Type::fct_return,
    _native(env),
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