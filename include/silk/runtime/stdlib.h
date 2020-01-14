#pragma once

#include "abstract/env.h"
#include "abstract/obj.h"

class StdLib {
  private:
  static void load_io(Environment<ObjectPtr>&);
  static void load_str(Environment<ObjectPtr>&);
  static void load_meta(Environment<ObjectPtr>&);
  static void load_maths(Environment<ObjectPtr>&);
  static void load_data(Environment<ObjectPtr>&);

  public:
  static void load_library(const std::string&, Environment<ObjectPtr>&);
};
