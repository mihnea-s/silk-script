#pragma once

#include <filesystem>
#include <istream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <silk/syntax/tree.h>

namespace silk {

/// A single unparsed source file in a package tree.
struct Source {
  std::string   path;
  std::istream &source;
};

/// A single parsed source file in a package tree.
struct Module {
  std::string                            path;
  std::vector<Token>                     tokens;
  std::vector<std::unique_ptr<st::Node>> tree;

  Module(const Module &) = delete;
  Module(Module &&)      = default;
};

/// Multiple source files make up one package as a unit.
struct Package {
  std::string                             name;
  std::unordered_map<std::string, Module> modules;
};

/// Read the source folder of a package.
auto read_package(std::filesystem::path) -> std::optional<Package>;

} // namespace silk
