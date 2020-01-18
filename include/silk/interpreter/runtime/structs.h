#pragma once

#include <any>
#include <map>
#include <vector>

#include "../ast/expr.h"
#include "../ast/stmt.h"
#include "../interfaces/obj.h"

#include "interpreter.h"

struct Struct : Object, Callable {
  std::string name;
  ObjectPtr   super;

  Expr::Expr ctor;
  Expr::Expr dtor;

  std::vector<Stmt::Stmt> fields;
  std::vector<Stmt::Stmt> methods;

  std::size_t               _arity;
  Interpreter&              _interp;
  Interpreter::Environment& _env;

  Struct(
    std::string name,
    ObjectPtr   super,

    Expr::Expr&              ctor,
    Expr::Expr&              dtor,
    std::vector<Stmt::Stmt>& fields,
    std::vector<Stmt::Stmt>& methods,

    Interpreter&              interp,
    Interpreter::Environment& env) :
      name(name),
      super(super),
      ctor(std::move(ctor)),
      dtor(std::move(dtor)),
      fields(std::move(fields)),
      methods(std::move(methods)),

      _interp(interp),
      _env(env) {
    try {
      auto& ctor = std::get<std::unique_ptr<Expr::Lambda>>(this->ctor);
      _arity     = ctor->parameters.size();
    } catch (std::bad_variant_access) { _arity = 0; }
  }

  std::string string() override;
  bool        truthy() override;

  ObjectPtr operator==(ObjectPtr&) override;

  std::size_t arity() const override;
  void        call(std::vector<ObjectPtr>&) override;
};

struct Instance : Object, Gettable {
  Struct&                              type;
  ObjectPtr                            super;
  std::map<std::string, ObjectPtr>     methods;
  Interpreter::Environment::SavedScope closure;

  Instance(
    Struct&                              type,
    ObjectPtr                            super,
    Interpreter::Environment::SavedScope closure) :
      type(type), super(super), methods(), closure(closure) {
  }

  ObjectPtr get(const std::string&) override;

  std::string string() override;
  bool        truthy() override;

  ObjectPtr operator==(ObjectPtr&) override;
};

struct NativeInstance : Object, Gettable {
  std::string string() override;
  bool        truthy() override;

  ObjectPtr operator==(ObjectPtr&) override;
};