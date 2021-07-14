#pragma once

#include <filesystem>
#include <fstream>
#include <istream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <silk/language/syntax_tree.h>

namespace silk {

template <class T>
using std_str_map = std::unordered_map<std::string, T>;

/// A single unparsed source file in a package tree.
struct Source {
  std::string   path;
  std::ifstream source;
};

/// Multiple source files make up one package source as a unit.
struct PackageSource {
  Source              main;
  std_str_map<Source> sources;
};

/// A single parsed source file in a package tree.
struct Module {
  std::string                            path;
  std::vector<std::unique_ptr<st::Node>> tree;

  Module(const Module &) = delete;
  Module(Module &&)      = default;
};

/// Multiple modules with a main module make up one package as a unit.
struct Package {
  Module              main;
  std_str_map<Module> modules;
};

/// User friendly token kind string
auto token_kind_string(TokenKind) -> std::string_view;

} // namespace silk
