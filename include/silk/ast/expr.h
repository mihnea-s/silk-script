#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "token.h"

namespace Stmt {

struct Empty;

struct Entrypoint;
struct Package;
struct Import;
struct Forwarding;
struct Export;

struct Variable;
struct Function;
struct Struct;
struct Constructor;
struct Destructor;

struct Loop;
struct LoopInterupt;
struct Conditional;
struct Block;
struct Return;
struct ExprStmt;

using Stmt = std::variant<
  std::unique_ptr<Empty>,

  std::unique_ptr<Entrypoint>,
  std::unique_ptr<Package>,
  std::unique_ptr<Import>,
  std::unique_ptr<Forwarding>,
  std::unique_ptr<Export>,

  std::unique_ptr<Variable>,
  std::unique_ptr<Function>,
  std::unique_ptr<Struct>,
  std::unique_ptr<Constructor>,
  std::unique_ptr<Destructor>,

  std::unique_ptr<Loop>,
  std::unique_ptr<LoopInterupt>,
  std::unique_ptr<Conditional>,
  std::unique_ptr<Block>,
  std::unique_ptr<Return>,
  std::unique_ptr<ExprStmt>>;

} // namespace Stmt

namespace Expr {

struct Empty;

struct Unary;
struct Binary;

struct IntLiteral;
struct DoubleLiteral;
struct BoolLiteral;
struct StringLiteral;
struct VidLiteral;

struct Identifier;
struct Assignment;
struct Grouping;
struct Call;
struct Lambda;

using Expr = std::variant<
  std::unique_ptr<Empty>,
  std::unique_ptr<Unary>,
  std::unique_ptr<Binary>,

  std::unique_ptr<IntLiteral>,
  std::unique_ptr<DoubleLiteral>,
  std::unique_ptr<BoolLiteral>,
  std::unique_ptr<StringLiteral>,
  std::unique_ptr<VidLiteral>,

  std::unique_ptr<Identifier>,
  std::unique_ptr<Assignment>,
  std::unique_ptr<Grouping>,
  std::unique_ptr<Call>,
  std::unique_ptr<Lambda>>;

struct Empty {};

struct Unary {
  const Token operation;
  const Expr  operand;

  Unary(const Token& operation, Expr&& operand) :
      operation(operation), operand(std::move(operand)) {
  }
};

struct Binary {
  const Expr  left_operand;
  const Token operation;
  const Expr  right_operand;

  Binary(
    Expr&& left_operand, const Token& operation, Expr&& right_operand) :
      left_operand(std::move(left_operand)),
      operation(operation),
      right_operand(std::move(right_operand)) {
  }
};

struct IntLiteral {
  const std::int64_t value;

  IntLiteral(const std::int64_t& value) : value(value) {
  }
};

struct DoubleLiteral {
  const double value;

  DoubleLiteral(const double& value) : value(value) {
  }
};

struct BoolLiteral {
  const bool value;

  BoolLiteral(const bool& value) : value(value) {
  }
};

struct StringLiteral {
  const std::string value;

  StringLiteral(const std::string& value) : value(value) {
  }
};

struct VidLiteral {};

struct Identifier {
  const std::string value;

  Identifier(const std::string& value) : value(value) {
  }
};

struct Assignment {
  const std::string name;
  const Expr        expr;

  Assignment(const std::string name, Expr&& expr) :
      name(name), expr(std::move(expr)) {
  }
};

struct Grouping {
  const Expr inner;

  Grouping(Expr&& inner) : inner(std::move(inner)) {
  }
};

struct Call {
  const Expr              callee;
  const std::vector<Expr> args;

  Call(Expr&& callee, std::vector<Expr>&& args) :
      callee(std::move(callee)), args(std::move(args)) {
  }
};

struct Lambda {
  const std::vector<std::string> parameters {};
  const Stmt::Stmt               body;

  Lambda(std::vector<std::string>& params, Stmt::Stmt&& body) :
      parameters(params), body(std::move(body)) {
  }
};

template <class T>
struct Visitor {
  template <class Ptr>
  auto operator()(Ptr&& ptr) noexcept -> T {
    return this->evaluate(*ptr.get());
  }

  auto evaluate_expr(const Expr& expr) noexcept -> T {
    return std::visit(*this, expr);
  }

  virtual T evaluate(const Empty&)  = 0;
  virtual T evaluate(const Unary&)  = 0;
  virtual T evaluate(const Binary&) = 0;

  virtual T evaluate(const IntLiteral&)    = 0;
  virtual T evaluate(const DoubleLiteral&) = 0;
  virtual T evaluate(const BoolLiteral&)   = 0;
  virtual T evaluate(const StringLiteral&) = 0;
  virtual T evaluate(const VidLiteral&)    = 0;

  virtual T evaluate(const Identifier&) = 0;
  virtual T evaluate(const Assignment&) = 0;
  virtual T evaluate(const Grouping&)   = 0;
  virtual T evaluate(const Call&)       = 0;
  virtual T evaluate(const Lambda&)     = 0;
};

} // namespace Expr
