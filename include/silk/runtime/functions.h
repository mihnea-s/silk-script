#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "../ast/stmt.h"

#include "interpreter.h"

#include "abstract/obj.h"

struct Interpreter;

struct Function : Object, Callable {
  private:
  using Arguments = std::vector<std::pair<std::string, std::string>>;

  const Stmt::Stmt _body;
  const Arguments  _args;

  Interpreter&                         _interp;
  Interpreter::Environment&            _env;
  Interpreter::Environment::SavedScope _closure;

  public:
  Function(
    Stmt::Stmt&      body,
    const Arguments& args,

    Interpreter&                         intr,
    Interpreter::Environment&            env,
    Interpreter::Environment::SavedScope closure) :
      _body(std::move(body)),
      _args(args),

      _interp(intr),
      _env(env),
      _closure(closure) {
  }

  std::size_t arity() const override;

  void call(std::vector<ObjectPtr>&) override;

  std::string string() override;
  bool        truthy() override;

  ObjectPtr operator==(ObjectPtr&) override;
};

struct VirtFunction : Object {
  std::string string() override;
  bool        truthy() override;

  ObjectPtr operator==(ObjectPtr&) override;
};

struct NativeFunction : Object, Callable {
  using NativeFct = std::function<ObjectPtr(std::vector<ObjectPtr>&)>;

  const NativeFct   _native;
  const std::size_t _args;

  NativeFunction(NativeFct&& native, std::size_t args) :
      _native(std::move(native)), _args(args) {
  }

  std::size_t arity() const override;

  void call(std::vector<ObjectPtr>&) override;

  std::string string() override;
  bool        truthy() override;

  ObjectPtr operator==(ObjectPtr&) override;
};
