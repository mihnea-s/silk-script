#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>

#include <fmt/core.h>

#include <silk/error.h>
#include <silk/runtime/abstract/obj.h>
#include <silk/runtime/functions.h>
#include <silk/runtime/primitives.h>
#include <silk/runtime/stdlib.h>
#include <silk/runtime/structs.h>

using Fct = std::function<ObjectPtr(std::vector<ObjectPtr>&)>;

constexpr int variable_args = -1;

auto make_fct(
  Interpreter::Environment& env,
  std::string               name,
  std::size_t               arg_count,
  Fct                       fct) {
  auto nativefct =
    ObjectPtr {new NativeFunction {std::move(fct), arg_count}};
  env.define(name, nativefct);
}

auto make_fct(std::size_t arg_count, Fct fct) {
  return ObjectPtr {new NativeFunction {std::move(fct), arg_count}};
}

struct StdFile : NativeInstance {
  std::fstream f;

  // clang-format off
  const std::map<std::string, ObjectPtr> methods = {
    {"write", make_fct(1, [&](auto& args) {
      auto str = args.at(0);
      f << str->string() << std::endl;
      return obj::make<Vid>();
    })},
    {"rword", make_fct(0, [&](auto& args) {
      auto line = std::string{};
      f >> line;
      return obj::make(line);
    })},
    {"rline", make_fct(0, [&](auto& args) {
      auto line = std::string{};
      std::getline(f, line);
      return obj::make(line);
    })},
    {"eof", make_fct(0, [&](auto& args) {
      return obj::make(f.eof());
    })}
  };
  // clang-format on

  StdFile(std::string file, std::ios_base::openmode mode) : f(file, mode) {
  }

  ObjectPtr get(const std::string& name) override {
    if (methods.find(name) == std::end(methods)) {
      return obj::make<Vid>();
    }
    return methods.at(name);
  }
};

struct StdList : NativeInstance {
  std::vector<ObjectPtr> _list;

  // clang-format off
  const std::map<std::string, ObjectPtr> methods = {
    {"at", make_fct(1, [&](auto& args) {
      auto index_obj = args.at(0);
      auto index = obj::cast_to<Integer>(index_obj);
      return _list.at(index.value);
    })},

    {"in", make_fct(2, [&](auto& args) {
      auto index_obj = args.at(0);
      auto index = obj::cast_to<Integer>(index_obj);
     
      auto old = _list.at(index.value);
      _list[index.value] = args.at(1);
      return old;
    })},

    {"add", make_fct(1, [&](auto& args) {
      _list.push_back(args.at(0));
      return args.at(0);
    })},

    {"addAll", make_fct(variable_args, [&](auto& args) {
      _list.insert(_list.end(), args.begin(), args.end());
      return obj::make((std::int64_t)args.size());
    })},

    {"len", make_fct(0, [&](auto& args) {
      return obj::make((std::int64_t)_list.size());
    })},

    {"rem", make_fct(1, [&](auto& args) {
      auto index_obj = args.at(0);
      auto index = obj::cast_to<Integer>(index_obj);
      auto removed = _list.at(index.value);
      _list.erase(_list.begin() + index.value);
      return removed;
    })},

    {"each", make_fct(1, [&](auto& args) {
      auto& applied = obj::cast_to<Callable>(args.at(0));
      for (auto& elem : _list) {
        auto arg = std::vector{elem};
        applied.call(arg);
      }
      return obj::make<Vid>();
    })},

    {"eachi", make_fct(1, [&](auto& args) {
      auto& applied = obj::cast_to<Callable>(args.at(0));
      for (std::int64_t i = 0; i < _list.size(); i++) {
        auto arg = std::vector{obj::make(i), _list[i]};
        applied.call(arg);
      }
      return obj::make<Vid>();
    })},

    {"sort", make_fct(0, [&](auto&) {
      std::sort(
        _list.begin(),
        _list.end(),
        [](auto& lobj, auto& robj) {
          auto& left = obj::cast_to<Orderable>(lobj);
          auto& right = obj::cast_to<Orderable>(robj);
          return left > right;
        }
      );
      return obj::make<Vid>();
    })},

    {"reduce", make_fct(2, [&](auto& args) {
      auto value = args.at(0);
      auto& applied = obj::cast_to<Callable>(args.at(1));

      for (auto& elem : _list) {
        auto arg = std::vector{value, elem};
        try {
          applied.call(arg);
        } catch (Interpreter::Interrupt& ret) {
          value = ret.value;
        }
      }

      return value;
    })},
  };
  // clang-format on

  StdList(std::vector<ObjectPtr>&& from) : _list(std::move(from)) {
  }

  ObjectPtr get(const std::string& name) override {
    if (methods.find(name) == std::end(methods)) {
      return obj::make<Vid>();
    }
    return methods.at(name);
  }
};

struct StdMap : NativeInstance {
  std::map<std::string, ObjectPtr> _map;

  // clang-format off
  const std::map<std::string, ObjectPtr> methods = {
    {"at", make_fct(1, [&](auto& args) {
      auto index_obj = args.at(0);
      auto index = obj::cast_to<String>(index_obj);
      return _map.at(index.value);
    })},

    {"add", make_fct(2, [&](auto& args) {
      auto index_obj = args.at(0);
      auto data = args.at(1);
      auto index = obj::cast_to<String>(index_obj);
      _map.insert_or_assign(index.value, data);
      return data;
    })},

    {"len", make_fct(0, [&](auto& args) {
      return obj::make((std::int64_t)_map.size());
    })},

    {"rem", make_fct(1, [&](auto& args) {
      auto index_obj = args.at(0);
      auto index = obj::cast_to<String>(index_obj);
      auto removed = _map.at(index.value);
      _map.erase(index.value);
      return removed;
    })},

    {"each", make_fct(1, [&](auto& args) {
      auto& applied = obj::cast_to<Callable>(args.at(0));
      for (auto& [key, val] : _map) {
        auto arg = std::vector{obj::make(key), val};
        applied.call(arg);
      }
      return obj::make<Vid>();
    })},
  };
  // clang-format on

  StdMap() : _map() {
  }

  ObjectPtr get(const std::string& name) override {
    if (methods.find(name) == std::end(methods)) {
      return obj::make<Vid>();
    }
    return methods.at(name);
  }
};

struct StdSBuilder : NativeInstance {
  std::ostringstream _ss;

  // clang-format off
  const std::map<std::string, ObjectPtr> methods = {
    {"add", make_fct(-1, [&](auto& args) {
      for (auto& arg: args) {
        _ss << arg->string();
      }

      return obj::make((std::int64_t)args.size());
    })},

    {"addln", make_fct(-1, [&](auto& args) {
      for (auto& arg: args) {
        _ss << arg->string() << std::endl;
      }

      return obj::make((std::int64_t)args.size());
    })},

    {"str", make_fct(0, [&](auto&) {
      return obj::make(_ss.str());
    })},

    {"clear", make_fct(0, [&](auto&) {
      _ss.clear();
      return obj::make<Vid>();
    })},
  };
  // clang-format on

  StdSBuilder() : _ss() {
  }

  ObjectPtr get(const std::string& name) override {
    if (methods.find(name) == std::end(methods)) {
      return obj::make<Vid>();
    }
    return methods.at(name);
  }
};

void StdLib::load_library(
  const std::string& name, Interpreter::Environment& env) {
  if (name.compare("std/io") == 0) return load_io(env);
  if (name.compare("std/str") == 0) return load_str(env);
  if (name.compare("std/meta") == 0) return load_meta(env);
  if (name.compare("std/maths") == 0) return load_maths(env);
  if (name.compare("std/data") == 0) return load_data(env);
  throw RuntimeError {"invalid standard library"};
}

void StdLib::load_io(Interpreter::Environment& env) {
  make_fct(env, "print", 1, [](auto& args) {
    auto str = args.at(0);
    fmt::print("{}", str->string());
    return obj::make<Vid>();
  });

  make_fct(env, "println", 1, [](auto& args) {
    auto str = args.at(0);
    fmt::print("{}\n", str->string());
    return obj::make<Vid>();
  });

  make_fct(env, "input", 0, [](auto&) {
    std::string in {};
    std::getline(std::cin, in);
    return obj::make(in);
  });

  make_fct(env, "open", 1, [](auto& args) {
    auto name     = args.at(0);
    auto name_str = obj::cast_to<String>(name);
    using namespace std;
    constexpr auto open_mode = fstream::out | fstream::in | fstream::app;
    auto file = std::make_shared<StdFile>(name_str.value, open_mode);
    return file;
  });

  make_fct(env, "create", 1, [](auto& args) {
    auto name     = args.at(0);
    auto name_str = obj::cast_to<String>(name);
    using namespace std;
    constexpr auto open_mode = fstream::out | fstream::in | fstream::trunc;
    auto file = std::make_shared<StdFile>(name_str.value, open_mode);
    return file;
  });
}

void StdLib::load_str(Interpreter::Environment& env) {
  make_fct(env, "searchs", 2, [](auto& args) {
    auto str  = args.at(0);
    auto find = args.at(1);

    auto res = str->string().find(find->string());
    if (res == std::string::npos) res = -1;

    return obj::make((std::int64_t)res);
  });

  make_fct(env, "cuts", 3, [](auto& args) {
    auto str = args.at(0);

    auto start_obj = args.at(1);
    auto end_obj   = args.at(2);

    auto start = obj::cast_to<Integer>(start_obj);
    auto end   = obj::cast_to<Integer>(end_obj);

    auto res = str->string().substr(start.value, end.value);
    return obj::make(res);
  });

  make_fct(env, "lens", 1, [](auto& args) {
    auto str_obj = args.at(0);
    auto str     = obj::cast_to<String>(str_obj);

    return obj::make((std::int64_t)str.value.size());
  });

  make_fct(env, "intConverts", 1, [](auto& args) {
    auto str_obj = args.at(0);
    auto str     = obj::cast_to<String>(str_obj);

    return obj::make((std::int64_t)std::atoi(str.value.data()));
  });

  make_fct(env, "realConverts", 1, [](auto& args) {
    auto str_obj = args.at(0);
    auto str     = obj::cast_to<String>(str_obj);

    return obj::make(std::atof(str.value.data()));
  });

  make_fct(env, "sbuilder", 0, [](auto& args) {
    return obj::make<StdSBuilder>();
  });
}

void StdLib::load_meta(Interpreter::Environment& env) {
}

void StdLib::load_maths(Interpreter::Environment& env) {
}

void StdLib::load_data(Interpreter::Environment& env) {
  make_fct(env, "list", variable_args, [](auto& args) {
    return std::make_shared<StdList>(std::move(args));
  });

  make_fct(env, "map", 0, [](auto&) { return std::make_shared<StdMap>(); });
}