#include <cassert>
#include <regex>
#include <sstream>
#include <string>

#include <fmt/core.h>
#include <fmt/format.h>

#include <silk/common/error.h>
#include <silk/common/token.h>
#include <silk/interpreter/analyzers/checker.h>
#include <silk/interpreter/ast/ast.h>
#include <silk/interpreter/ast/expr.h>
#include <silk/interpreter/ast/stmt.h>

auto Checker::add_error(const std::string& msg) -> void {
  _errors.push_back(ParsingError {
    Severity::error,
    msg,
    {},
  });
}

auto Checker::push_scope() -> void {
  _scopes.emplace_back();
}

auto Checker::pop_scope() -> void {
  _scopes.pop_back();
}

auto Checker::var_type(const std::string& name) -> std::string {
  for (auto it = _scopes.rbegin(); it != _scopes.rend(); it++) {
    if ((*it).find(name) != (*it).end()) return (*it).at(name);
  }

  return "vid";
}

auto Checker::evaluate(const Expr::Unary& expr) -> std::string {
  auto operand = evaluate_expr(expr.operand);

  switch (expr.operation.type()) {

    case TokenType::sym_bang: return "bool";

    case TokenType::sym_tilde: {
      if (not_dyn(operand) && operand != "int") {
        add_error(fmt::format("bitwise not applied on {}", operand));
        return "vid";
      }
      return "int";
    }

    case TokenType::sym_minus: {
      if (not_dyn(operand) && not_in(operand, "real", "int")) {
        add_error(fmt::format("negation applied on {}", operand));
        return "vid";
      };
      return "real";
    };

    default: return "vid";
  }
}

auto Checker::evaluate(const Expr::Binary& expr) -> std::string {
  evaluate_expr(expr.left_operand);
  evaluate_expr(expr.right_operand);
  return "real";
}

auto Checker::evaluate(const Expr::IntLiteral& integer) -> std::string {
  return "int";
}

auto Checker::evaluate(const Expr::DoubleLiteral& dbl) -> std::string {
  return "real";
}

auto Checker::evaluate(const Expr::BoolLiteral& boolean) -> std::string {
  return "bool";
}

auto Checker::evaluate(const Expr::StringLiteral& str) -> std::string {
  return "str";
}

auto Checker::evaluate(const Expr::Vid&) -> std::string {
  return "vid";
}

auto Checker::evaluate(const Expr::Assignment& assignment) -> std::string {
  auto assign   = evaluate_expr(assignment.expr);
  auto assignee = _scopes.back()[assignment.name];
  if (not_dyn(assign, assignee) && assign != assignee) {
    add_error(fmt::format(
      "type mismatch: `{}` of type `{}` was assigned `{}`",
      assignment.name,
      assignee,
      assign));
  }
  return nullptr;
}

auto Checker::evaluate(const Expr::Identifier& id) -> std::string {
  return var_type(id.value);
}

auto Checker::evaluate(const Expr::Grouping& group) -> std::string {
  return evaluate_expr(group.inner);
}

auto Checker::evaluate(const Expr::Call& call) -> std::string {
  return "";
}

auto Checker::evaluate(const Expr::Get& get) -> std::string {
  return "";
}

auto Checker::evaluate(const Expr::Lambda& lambda) -> std::string {
  return "callable";
}

auto Checker::execute(const Stmt::Empty&) -> std::nullptr_t {
  return nullptr;
}

auto Checker::execute(const Stmt::Main&) -> std::nullptr_t {
  _scopes.emplace_back();
  return nullptr;
}

auto Checker::execute(const Stmt::Package& pkg) -> std::nullptr_t {
  return nullptr;
}

auto Checker::execute(const Stmt::Import& imprt) -> std::nullptr_t {
  return nullptr;
}

auto Checker::execute(const Stmt::Variable& var) -> std::nullptr_t {
  _scopes.back()[var.name] = var.type;
  auto init_type           = evaluate_expr(var.init);
  if (init_type != var.type) {
    add_error(fmt::format(
      "type mismatch: {} of type {} was assigned {}",
      var.name,
      var.type,
      init_type));
  }
  return nullptr;
}

auto Checker::execute(const Stmt::Function& fct) -> std::nullptr_t {
  return nullptr;
}

auto Checker::execute(const Stmt::Struct&) -> std::nullptr_t {
  return nullptr;
}

auto Checker::execute(const Stmt::Loop& l) -> std::nullptr_t {
  evaluate_expr(l.clause);
  execute_stmt(l.body);
  return nullptr;
}

auto Checker::execute(const Stmt::Conditional& cond) -> std::nullptr_t {
  evaluate_expr(cond.clause);
  execute_stmt(cond.true_stmt);
  execute_stmt(cond.false_stmt);
  return nullptr;
}

auto Checker::execute(const Stmt::Block& blk) -> std::nullptr_t {
  push_scope();
  for (auto& stmt : blk.body) {
    execute_stmt(stmt);
  }
  pop_scope();
  return nullptr;
}

auto Checker::execute(const Stmt::Interrupt& r) -> std::nullptr_t {
  evaluate_expr(r.ret);
  return nullptr;
}

auto Checker::execute(const Stmt::ExprStmt& e) -> std::nullptr_t {
  evaluate_expr(e.expr);
  return nullptr;
}

auto Checker::has_error() const -> bool {
  return !_errors.empty();
}

auto Checker::clear_errors() -> void {
  _errors.clear();
}

auto Checker::errors() -> std::vector<ParsingError>& {
  return _errors;
}

auto Checker::check(AST& ast) -> void {
  try {
    ast.execute_with(*this);
  } catch (ParsingError e) {
    _errors.push_back(e);
    return;
  }
}