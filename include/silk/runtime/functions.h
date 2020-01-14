#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "../ast/stmt.h"

#include "abstract/env.h"
#include "abstract/obj.h"

struct Interpreter;

struct Function : Object, Callable {
  private:
  const Stmt::Stmt               _body;
  const std::vector<std::string> _args;

  Interpreter& _intr;

  public:
  Function(
    Stmt::Stmt&                     body,
    const std::vector<std::string>& args,
    Interpreter&                    intr) :
      _body(std::move(body)), _args(args), _intr(intr) {
  }

  std::size_t arity() const override;
  std::string argument(std::size_t) override;

  void call(Environment<ObjectPtr>&) override;

  std::string string() override;
  bool        truthy() override;

  ObjectPtr operator==(ObjectPtr&) override;
};

struct NativeFunction : Object, Callable {
  using NativeFct = std::function<ObjectPtr(Environment<ObjectPtr>&)>;

  const NativeFct                _native;
  const std::vector<std::string> _args;

  NativeFunction(NativeFct&& native, std::vector<std::string>&& args) :
      _native(std::move(native)), _args(std::move(args)) {
  }

  std::size_t arity() const override;
  std::string argument(std::size_t) override;

  void call(Environment<ObjectPtr>&) override;

  std::string string() override;
  bool        truthy() override;

  ObjectPtr operator==(ObjectPtr&) override;
};
