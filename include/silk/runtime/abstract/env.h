#pragma once

#include <string>

template <class T>
struct Environment {
  virtual void push_scope() = 0;
  virtual void pop_scope()  = 0;

  virtual bool exists(const std::string& name)            = 0;
  virtual T    get(const std::string& name)               = 0;
  virtual T    assign(const std::string& name, T&& value) = 0;
};
