#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include "../error.h"

#include "../ast/expr.h"
#include "../ast/stmt.h"

#include "abstract/obj.h"

namespace obj {

ObjectPtr make(std::int64_t);
ObjectPtr make(double);
ObjectPtr make(bool);
ObjectPtr make(std::string);

template <class T, class... Args>
auto make(Args&&... args) -> ObjectPtr {
  return std::make_shared<T>(args...);
}

template <class T>
auto cast_to(ObjectPtr& obj) -> T& {
  try {
    return dynamic_cast<T&>(*obj);
  } catch (std::bad_cast) { throw RuntimeError {"invalid cast of object"}; }
}

template <class D, class B>
auto cast_to(B& base) -> D& {
  try {
    return dynamic_cast<D&>(base);
  } catch (std::bad_cast) { throw RuntimeError {"invalid cast of object"}; }
}

template <class T>
auto try_cast(ObjectPtr& obj) -> std::optional<std::reference_wrapper<T>> {
  try {
    return std::ref(dynamic_cast<T&>(*obj));
  } catch (std::bad_cast) { return std::nullopt; }
}

template <class D, class B>
auto try_cast(B& base) -> std::optional<std::reference_wrapper<D>> {
  try {
    return std::ref(dynamic_cast<D&>(base));
  } catch (std::bad_cast) { return std::nullopt; }
}

} // namespace obj

struct Integer : Object, Orderable, Numerical, BitModifiable, Concatenable {
  std::int64_t value;

  Integer(std::int64_t value) : value(value) {
  }

  std::string string() override;
  bool        truthy() override;

  ObjectPtr operator==(ObjectPtr&) override;

  ObjectPtr operator<(Orderable&) override;
  ObjectPtr operator>(Orderable&) override;

  ObjectPtr operator>=(Orderable&) override;
  ObjectPtr operator<=(Orderable&) override;

  ObjectPtr operator~() override;
  ObjectPtr operator-() override;

  ObjectPtr operator+(Concatenable&) override;
  ObjectPtr operator-(Numerical&) override;
  ObjectPtr operator*(Concatenable&)override;
  ObjectPtr operator/(Numerical&) override;
  ObjectPtr starstar(Numerical&) override;
  ObjectPtr slashslash(Numerical&) override;

  ObjectPtr operator%(BitModifiable&) override;
  ObjectPtr operator&(BitModifiable&)override;
  ObjectPtr operator|(BitModifiable&) override;
  ObjectPtr operator^(BitModifiable&) override;
};

struct Double : Object, Orderable, Numerical, Concatenable {
  double value;

  Double(double value) : value(value) {
  }

  std::string string() override;
  bool        truthy() override;

  ObjectPtr operator==(ObjectPtr&) override;

  ObjectPtr operator<(Orderable&) override;
  ObjectPtr operator>(Orderable&) override;

  ObjectPtr operator>=(Orderable&) override;
  ObjectPtr operator<=(Orderable&) override;

  ObjectPtr operator-() override;
  ObjectPtr operator+(Concatenable&) override;
  ObjectPtr operator-(Numerical&) override;
  ObjectPtr operator*(Concatenable&)override;
  ObjectPtr operator/(Numerical&) override;
  ObjectPtr starstar(Numerical&) override;
  ObjectPtr slashslash(Numerical&) override;
};

struct Boolean : Object {
  bool value;

  Boolean(bool value) : value(value) {
  }

  std::string string() override;
  bool        truthy() override;

  ObjectPtr operator==(ObjectPtr&) override;
};

struct String : Object, Orderable, Concatenable {
  std::string value;

  String(std::string value) : value(value) {
  }

  std::string string() override;
  bool        truthy() override;

  ObjectPtr operator==(ObjectPtr&) override;

  ObjectPtr operator<(Orderable&) override;
  ObjectPtr operator>(Orderable&) override;

  ObjectPtr operator>=(Orderable&) override;
  ObjectPtr operator<=(Orderable&) override;

  ObjectPtr operator+(Concatenable&) override;
  ObjectPtr operator*(Concatenable&)override;
};

struct Vid : Object {
  std::string string() override;
  bool        truthy() override;

  ObjectPtr operator==(ObjectPtr&) override;
};
