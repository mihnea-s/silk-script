#pragma once

#include <memory>
#include <variant>

// this file includes forward declarations
// to everything in the Expr namespace (except Visitor)
// see 'stmt_decl.h' for motivation

namespace Expr {

struct Unary;
struct Binary;

struct IntLiteral;
struct DoubleLiteral;
struct BoolLiteral;
struct StringLiteral;
struct Vid;
struct Lambda;

struct Identifier;
struct Assignment;
struct Grouping;
struct Call;
struct Get;

// see Stmt::Stmt in 'stmt_decl.h'

using Expr = std::variant<
  std::unique_ptr<Unary>,
  std::unique_ptr<Binary>,

  std::unique_ptr<IntLiteral>,
  std::unique_ptr<DoubleLiteral>,
  std::unique_ptr<BoolLiteral>,
  std::unique_ptr<StringLiteral>,
  std::unique_ptr<Vid>,
  std::unique_ptr<Lambda>,

  std::unique_ptr<Identifier>,
  std::unique_ptr<Assignment>,
  std::unique_ptr<Grouping>,
  std::unique_ptr<Call>,
  std::unique_ptr<Get>>;

} // namespace Expr