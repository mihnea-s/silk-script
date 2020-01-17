#include <optional>
#include <variant>

#include <fmt/core.h>
#include <fmt/format.h>

#include <silk/ast/expr.h>
#include <silk/ast/stmt.h>
#include <silk/runtime/functions.h>
#include <silk/runtime/interpreter.h>
#include <silk/runtime/primitives.h>
#include <silk/runtime/structs.h>

// Struct -----------------------------

std::string Struct::string() {
  return name;
}

bool Struct::truthy() {
  return true;
}

ObjectPtr Struct::operator==(ObjectPtr& other) {
  return obj::make(false);
}

std::size_t Struct::arity() const {
  return _arity;
}

void Struct::call(std::vector<ObjectPtr>& args) {
  auto parent = obj::make<Vid>();

  if (super) {
    try {
      auto super_ctor = obj::try_cast<Struct>(super);
      if (super_ctor.has_value()) { super_ctor->get().call(args); }
    } catch (Interpreter::Interrupt& ret) { parent = ret.value; }
  }

  auto instanceptr = new Instance(*this, parent, _env.save_scope());
  auto instance    = std::shared_ptr<Object>(instanceptr);

  auto scp = _env.new_scope();
  _env.define("own", instance);
  _env.define("sup", parent);

  for (auto& field : this->fields) {
    _interp.execute_stmt(field);
  }

  instanceptr->closure = _env.save_scope();

  for (auto& method : this->methods) {
    auto& metd = std::get<std::unique_ptr<Stmt::Function>>(method);
    auto& body = const_cast<Stmt::Stmt&>(metd->body);
    auto  obj  = obj::make<Function>(
      body, metd->parameters, _interp, _env, instanceptr->closure);
    instanceptr->methods[metd->name] = obj;
  }

  try {
    auto& ctor = std::get<std::unique_ptr<Expr::Lambda>>(this->ctor);

    if (args.size() != _arity) {
      throw RuntimeError {
        "function called with the wrong number of arguments"};
    }

    for (auto i = 0; i < _arity; i++) {
      _env.define(ctor->parameters[i].first, args[i]);
    }

    _interp.execute_stmt(ctor->body);

  } catch (std::bad_variant_access) {
    // if ctor is not a lambda catch the error and do nothing
  }

  // return the instance by throwing
  // a returning interrupt
  throw Interpreter::Interrupt {
    instance,
    Stmt::Interrupt::Type::ret,
  };
}

// Instance ---------------------------

std::string Instance::string() {
  return fmt::format("[instance of {}]", type.string());
}

bool Instance::truthy() {
  return true;
}

ObjectPtr Instance::operator==(ObjectPtr& other_obj) {
  auto& other = obj::cast_to<Instance&>(other_obj);
  return obj::make(this == &other);
}

ObjectPtr Instance::get(const std::string& name) {
  if (methods.find(name) != std::end(methods)) { return methods[name]; }

  auto sup = obj::try_cast<Instance>(super);

  if (sup.has_value()) { return sup->get().get(name); }

  return obj::make<Vid>();
}

std::string NativeInstance::string() {
  return "[native instance]";
}

bool NativeInstance::truthy() {
  return true;
}

ObjectPtr NativeInstance::operator==(ObjectPtr& other_obj) {
  auto& other = obj::cast_to<NativeInstance&>(other_obj);
  return obj::make(this == &other);
}