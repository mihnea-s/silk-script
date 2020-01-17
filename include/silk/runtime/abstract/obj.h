#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct Object;

using ObjectPtr = std::shared_ptr<Object>;

struct Object {
  virtual std::string string() = 0;
  virtual bool        truthy() = 0;

  virtual ObjectPtr operator==(ObjectPtr&) = 0;
};

struct Numerical {
  virtual ObjectPtr operator-()            = 0;
  virtual ObjectPtr operator-(Numerical&)  = 0;
  virtual ObjectPtr operator/(Numerical&)  = 0;
  virtual ObjectPtr starstar(Numerical&)   = 0;
  virtual ObjectPtr slashslash(Numerical&) = 0;
};

struct BitModifiable {
  virtual ObjectPtr operator~()               = 0;
  virtual ObjectPtr operator&(BitModifiable&) = 0;
  virtual ObjectPtr operator|(BitModifiable&) = 0;
  virtual ObjectPtr operator^(BitModifiable&) = 0;
  virtual ObjectPtr operator%(BitModifiable&) = 0;
};

struct Orderable {
  virtual ObjectPtr operator<(Orderable&)  = 0;
  virtual ObjectPtr operator>(Orderable&)  = 0;
  virtual ObjectPtr operator>=(Orderable&) = 0;
  virtual ObjectPtr operator<=(Orderable&) = 0;
};

struct Concatenable {
  virtual ObjectPtr operator+(Concatenable&) = 0;
  virtual ObjectPtr operator*(Concatenable&) = 0;
};

struct Callable {
  virtual std::size_t arity() const                 = 0;
  virtual void        call(std::vector<ObjectPtr>&) = 0;
};

struct Gettable {
  virtual ObjectPtr get(const std::string&) = 0;
};