#include <chrono>

#include <fmt/core.h>

#include <silk/error.h>
#include <silk/runtime/functions.h>
#include <silk/runtime/objects.h>
#include <silk/runtime/stdlib.h>

#include <silk/runtime/abstract/env.h>
#include <silk/runtime/abstract/obj.h>

void StdLib::load_library(
  const std::string& name, Environment<ObjectPtr>& env) {
  if (name.compare("std/io") == 0) return load_io(env);
  if (name.compare("std/str") == 0) return load_str(env);
  if (name.compare("std/meta") == 0) return load_meta(env);
  if (name.compare("std/maths") == 0) return load_maths(env);
  if (name.compare("std/data") == 0) return load_data(env);
  throw RuntimeError {"invalid standard library"};
}

void StdLib::load_io(Environment<ObjectPtr>& env) {
  env.assign(
    "print",

    ObjectPtr {
      new NativeFunction {
        [](Environment<ObjectPtr>& env) {
          auto str = env.get("str_to_print");
          fmt::print("{}", str->string());
          return obj::make<Vid>();
        },
        {"str_to_print"},
      },
    });

  env.assign(
    "println",

    ObjectPtr {
      new NativeFunction {
        [](Environment<ObjectPtr>& env) {
          auto str = env.get("str_to_print");
          fmt::print("{}\n", str->string());
          return obj::make<Vid>();
        },
        {"str_to_print"},
      },
    });

  env.assign(
    "now_millis",

    ObjectPtr {
      new NativeFunction {
        [](Environment<ObjectPtr>& env) {
          using namespace std::chrono;
          auto time   = system_clock::now().time_since_epoch();
          auto millis = duration_cast<milliseconds>(time);
          return obj::make(millis.count());
        },
        {},
      },
    });
}

void StdLib::load_str(Environment<ObjectPtr>& env) {
}

void StdLib::load_meta(Environment<ObjectPtr>& env) {
}

void StdLib::load_maths(Environment<ObjectPtr>& env) {
}

void StdLib::load_data(Environment<ObjectPtr>& env) {
}