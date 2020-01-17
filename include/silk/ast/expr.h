#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "expr_decl.h"
#include "stmt_decl.h"
#include "token.h"

// the structs in this namespace
// only hold data, they have no behaviour

namespace Expr {

// used to hold unary expressions
// (e.g. '!myPredicate')
struct Unary {
  const Token operation;
  const Expr  operand;

  Unary(const Token& operation, Expr&& operand) :
      operation(operation), operand(std::move(operand)) {
  }
};

// used to hold binary expressions
// with two operands (e.g. 'x + z')
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

// used to hold the value of a integer literal
struct IntLiteral {
  const std::int64_t value;

  IntLiteral(const std::int64_t& value) : value(value) {
  }
};

// used to hold the value of a double literal
struct DoubleLiteral {
  const double value;

  DoubleLiteral(const double& value) : value(value) {
  }
};

// used to hold the value of a boolean literal
struct BoolLiteral {
  const bool value;

  BoolLiteral(const bool& value) : value(value) {
  }
};

// used to hold the value of a string literal
struct StringLiteral {
  const std::string value;

  StringLiteral(const std::string& value) : value(value) {
  }
};

// the 'null' of the language
struct Vid {};

// holds nameless function expressions (i.e. lambdas)
// (e.g. 'fct { return true; }')
// (note: lambdas cannot be arrow functions)
struct Lambda {
  using Parameters = std::vector<std::pair<std::string, std::string>>;

  const Parameters parameters {};
  const Stmt::Stmt body;

  Lambda(Parameters& params, Stmt::Stmt&& body) :
      parameters(params), body(std::move(body)) {
  }
};

// stores raw words from the script file
// (e.g. names, types)
struct Identifier {
  const std::string value;

  Identifier(const std::string& value) : value(value) {
  }
};

// assignment expressions to the current environment scope
// name cannot be in Parser::restricted_names
// (e.g. 'x = 23')
struct Assignment {
  const std::string name;
  const Expr        expr;

  Assignment(const std::string name, Expr&& expr) :
      name(name), expr(std::move(expr)) {
  }
};

// wrapper around an expression surrounded by parenthesis
// (e.g. '(2 + 3)')
struct Grouping {
  const Expr inner;

  Grouping(Expr&& inner) : inner(std::move(inner)) {
  }
};

// Calls to callable objects
// (e.g. 'myFunctions(x, y, z);')
struct Call {
  const Expr              callee;
  const std::vector<Expr> args;

  Call(Expr&& callee, std::vector<Expr>&& args) :
      callee(std::move(callee)), args(std::move(args)) {
  }
};

// Get expressions from structs
// (e.g. 'player.UpdateHealth();')
struct Get {
  const Expr        from;
  const std::string property;

  Get(Expr&& from, std::string& property) :
      from(std::move(from)), property(property) {
  }
};

// interface for objects used to evaluate expressions
// also visitor pattern boilerplate
template <class T>
struct Visitor {

  // since variant uses unique pointers we
  // need a wrapper that will automatically
  // dereference the pointer and call the
  // appropriate function
  template <class Ptr>
  auto operator()(Ptr&& ptr) noexcept -> T {
    return this->evaluate(*ptr.get());
  }

  // helper function to evaluate statements
  auto evaluate_expr(const Expr& expr) noexcept -> T {
    return std::visit(*this, expr);
  }

  // Visitor pattern boilerplate
  virtual T evaluate(const Unary&)  = 0;
  virtual T evaluate(const Binary&) = 0;

  virtual T evaluate(const IntLiteral&)    = 0;
  virtual T evaluate(const DoubleLiteral&) = 0;
  virtual T evaluate(const BoolLiteral&)   = 0;
  virtual T evaluate(const StringLiteral&) = 0;
  virtual T evaluate(const Vid&)           = 0;
  virtual T evaluate(const Lambda&)        = 0;

  virtual T evaluate(const Identifier&) = 0;
  virtual T evaluate(const Assignment&) = 0;
  virtual T evaluate(const Grouping&)   = 0;
  virtual T evaluate(const Call&)       = 0;
  virtual T evaluate(const Get&)        = 0;
};

} // namespace Expr
