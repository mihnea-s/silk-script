#pragma once

#include "abstract/obj.h"

struct Interupt {
  enum class Type {
    loop_continue,
    loop_break,
    fct_return,
  };

  Type      type;
  ObjectPtr ret;
};
