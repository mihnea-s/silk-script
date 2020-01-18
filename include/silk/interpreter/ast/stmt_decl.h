#pragma once

#include <memory>
#include <variant>

// this file includes forward declarations for
// everything in the Stmt namespace (except Visitor)
// this separation is required for statements to include
// expressions and vice-versa (otherwise including both
// files in each other would create a recursive include)

namespace Stmt {

struct Empty;

struct Main;
struct Package;
struct Import;

struct Variable;
struct Function;
struct Struct;

struct Loop;
struct Conditional;
struct Block;
struct Interrupt;
struct ExprStmt;

// Stmt uses unique pointers to satisfy the
// requirement of a complete type

using Stmt = std::variant<
  std::unique_ptr<Empty>,

  std::unique_ptr<Main>,
  std::unique_ptr<Package>,

  std::unique_ptr<Import>,

  std::unique_ptr<Variable>,
  std::unique_ptr<Function>,
  std::unique_ptr<Struct>,

  std::unique_ptr<Loop>,
  std::unique_ptr<Conditional>,
  std::unique_ptr<Block>,
  std::unique_ptr<Interrupt>,
  std::unique_ptr<ExprStmt>>;

} // namespace Stmt