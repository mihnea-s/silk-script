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

auto Checker::declare_var(const std::string& name, SilkType type) -> void {
  _variables.insert_or_assign(name, type);
}

auto Checker::variable_type(const std::string& name) -> SilkType {
  while (_variables.find(name) == _variables.end()) {
    throw_error("reference to undefined variable `{}`", name);
  }

  return _variables[name];
}

auto Checker::evaluate(const Expr::Unary& expr) -> SilkType {
  auto operand = evaluate_expr(expr.operand);

  switch (expr.operation.type()) {

    case TokenType::sym_bang: return SilkType::BOOLEAN;

    case TokenType::sym_tilde: {
      if (!of_type_or_dyn(operand, SilkType::INTEGER)) {
        throw_error("invalid operator to operator tilde");
      }

      return SilkType::INTEGER;
    }

    case TokenType::sym_minus: {
      if (!of_type_or_dyn(operand, SilkType::REAL, SilkType::INTEGER)) {
        throw_error("invalid operator to operator minus");
      }

      return operand;
    };

    default: return SilkType::DYNAMIC;
  }
}

auto Checker::evaluate(const Expr::Binary& expr) -> SilkType {
  evaluate_expr(expr.left_operand);
  evaluate_expr(expr.right_operand);
  return SilkType::REAL;
}

auto Checker::evaluate(const Expr::IntLiteral& integer) -> SilkType {
  return SilkType::INTEGER;
}

auto Checker::evaluate(const Expr::DoubleLiteral& dbl) -> SilkType {
  return SilkType::REAL;
}

auto Checker::evaluate(const Expr::BoolLiteral& boolean) -> SilkType {
  return SilkType::BOOLEAN;
}

auto Checker::evaluate(const Expr::StringLiteral& str) -> SilkType {
  return SilkType::STRING;
}

auto Checker::evaluate(const Expr::Vid&) -> SilkType {
  return SilkType::DYNAMIC;
}

auto Checker::evaluate(const Expr::Assignment& assignment) -> SilkType {
  auto assign_type = evaluate_expr(assignment.expr);
  auto var_type    = variable_type(assignment.name);

  if (!of_type_or_dyn(var_type, assign_type)) {
    throw_error(
      "type mismatch: `{}` of type `{}` was assigned `{}`",
      assignment.name,
      str_from_type(var_type),
      str_from_type(assign_type));
  }

  return assign_type;
}

auto Checker::evaluate(const Expr::Identifier& id) -> SilkType {
  return variable_type(id.value);
}

auto Checker::evaluate(const Expr::Grouping& group) -> SilkType {
  return evaluate_expr(group.inner);
}

auto Checker::evaluate(const Expr::Call& call) -> SilkType {
  auto function = evaluate_expr(call.callee);

  if (!of_type_or_dyn(function, SilkType::CALLABLE)) {
    throw_error("type `{}` is not callable", str_from_type(function));
  }

  return SilkType::DYNAMIC;
}

auto Checker::evaluate(const Expr::Get& get) -> SilkType {
  auto instance = evaluate_expr(get.from);

  if (!of_type_or_dyn(instance, SilkType::INSTANCE)) {
    throw_error("type `{}` is not an instance", str_from_type(instance));
  }

  return SilkType::DYNAMIC;
}

auto Checker::evaluate(const Expr::Lambda& lambda) -> SilkType {
  return SilkType::CALLABLE;
}

auto Checker::execute(const Stmt::Empty&) -> std::nullptr_t {
  return nullptr;
}

auto Checker::execute(const Stmt::Main&) -> std::nullptr_t {
  return nullptr;
}

auto Checker::execute(const Stmt::Package& pkg) -> std::nullptr_t {
  return nullptr;
}

auto Checker::execute(const Stmt::Import& imprt) -> std::nullptr_t {
  return nullptr;
}

auto Checker::execute(const Stmt::Variable& var) -> std::nullptr_t {
  declare_var(var.name, type_from_str(var.type));

  auto init_type = evaluate_expr(var.init);
  if (!of_type_or_dyn(init_type, variable_type(var.name))) {
    throw_error(
      "type mismatch: `{}` of type `{}` was initialized with `{}`",
      var.name,
      str_from_type(variable_type(var.name)),
      str_from_type(init_type));
  }

  return nullptr;
}

auto Checker::execute(const Stmt::Function& fct) -> std::nullptr_t {
  declare_var(fct.name, SilkType::CALLABLE);

  execute_stmt(fct.body);

  return nullptr;
}

auto Checker::execute(const Stmt::Struct& strct) -> std::nullptr_t {
  // TODO
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
  for (auto& stmt : blk.body) {
    execute_stmt(stmt);
  }
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