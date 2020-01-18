#pragma once

#include "../interfaces/obj.h"

#include "interpreter.h"

// the stdlib includes functions written in C++ that
// silk can call, standard library packages are prefixed
// with 'std'

class StdLib {
  private:
  static void load_io(Interpreter::Environment&);
  static void load_str(Interpreter::Environment&);
  static void load_meta(Interpreter::Environment&);
  static void load_maths(Interpreter::Environment&);
  static void load_data(Interpreter::Environment&);

  public:
  static void load_library(const std::string&, Interpreter::Environment&);
};
