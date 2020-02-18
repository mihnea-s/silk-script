#include <silk/common/checker.h>
#include <silk/common/error.h>

auto Checker::declare_var(const std::string& name, SilkType type) -> void {
  _variables.insert_or_assign(name, type);
}

auto Checker::variable_type(const std::string& name) -> SilkType {
  if (_variables.find(name) == _variables.end()) { return SilkType::DYNAMIC; }
  return _variables[name];
}

auto Checker::stmt_result() const noexcept -> CheckerExprResult {
  return {SilkType::NONE, false};
}

CheckerExprResult Checker::evaluate(const Unary& node) {
  auto operand = visit_node(node.operand);

  switch (node.operation) {

    case TokenType::sym_bang: return {SilkType::BOOLEAN, false};

    case TokenType::sym_tilde: {
      if (!of_type_or_dyn(operand.type, SilkType::INTEGER)) {
        throw report_error(node.location, "invalid operator to operator tilde");
      }

      return {SilkType::INTEGER, false};
    }

    case TokenType::sym_minus: {
      if (!of_type_or_dyn(operand.type, SilkType::REAL, SilkType::INTEGER)) {
        throw report_error(node.location, "invalid operator to operator minus");
      }

      return {operand.type, false};
    };

    default: return {SilkType::DYNAMIC, false};
  }
}

CheckerExprResult Checker::evaluate(const Binary&) {
  return {SilkType::DYNAMIC, false};
}

CheckerExprResult Checker::evaluate(const IntLiteral&) {
  return {SilkType::INTEGER, false};
}

CheckerExprResult Checker::evaluate(const RealLiteral&) {
  return {SilkType::REAL, false};
}

CheckerExprResult Checker::evaluate(const BoolLiteral&) {
  return {SilkType::BOOLEAN, false};
}

CheckerExprResult Checker::evaluate(const StringLiteral&) {
  return {SilkType::STRING, false};
}

CheckerExprResult Checker::evaluate(const Vid&) {
  return {SilkType::DYNAMIC, false};
}

CheckerExprResult Checker::evaluate(const Constant& node) {
  return {SilkType::REAL, false};
}

CheckerExprResult Checker::evaluate(const Lambda&) {
  return {SilkType::CALLABLE, false};
}

CheckerExprResult Checker::evaluate(const IdentifierRef& node) {
  return {variable_type(node.identifier), true};
}

CheckerExprResult Checker::evaluate(const IdentifierVal& node) {
  return {variable_type(node.identifier), false};
}

CheckerExprResult Checker::evaluate(const Assignment& node) {
  auto assign = visit_node(node.assignment);
  auto target = visit_node(node.target);

  if (!target.assignable) {
    throw report_error(node.location, "cannot assign to expression");
  }

  if (!of_type_or_dyn(target.type, assign.type)) {
    throw report_error(
      node.location,
      "type mismatch: variable of type `{}` was assigned `{}`",
      str_from_type(target.type),
      str_from_type(assign.type));
  }

  return {assign.type, false};
}

CheckerExprResult Checker::evaluate(const Grouping& node) {
  return visit_node(node.inner);
}

CheckerExprResult Checker::evaluate(const ConstExpr& node) {
  return visit_node(node.inner);
}

CheckerExprResult Checker::evaluate(const Call& node) {
  auto callee = visit_node(node.target);

  if (!of_type_or_dyn(callee.type, SilkType::CALLABLE)) {
    throw report_error(
      node.location, "tried calling type `{}`", str_from_type(callee.type));
  }

  return {SilkType::DYNAMIC, false};
}

CheckerExprResult Checker::evaluate(const Access& node) {
  auto target = visit_node(node.target);

  if (!of_type_or_dyn(target.type, SilkType::INSTANCE)) {
    throw report_error(
      node.location, "tried accessing type `{}`", str_from_type(target.type));
  }

  return {SilkType::DYNAMIC, false};
}

CheckerExprResult Checker::execute(const Empty&) {
  return stmt_result();
}

CheckerExprResult Checker::execute(const Package&) {
  return stmt_result();
}

CheckerExprResult Checker::execute(const ExprStmt& node) {
  visit_node(node.expression);
  return stmt_result();
}

CheckerExprResult Checker::execute(const Variable& node) {
  auto var_type = type_from_str(node.type);
  auto init     = visit_node(node.initializer);

  declare_var(node.name, var_type);

  if (!of_type_or_dyn(init.type, var_type)) {
    throw report_error(
      node.location,
      "type mismatch: `{}` of type `{}` was initialized with `{}`",
      node.name,
      str_from_type(var_type),
      str_from_type(init.type));
  }

  return stmt_result();
}

CheckerExprResult Checker::execute(const Function&) {
  return stmt_result();
}

CheckerExprResult Checker::execute(const Struct&) {
  return stmt_result();
}

CheckerExprResult Checker::execute(const Loop& node) {
  visit_node(node.clause);
  visit_node(node.body);
  return stmt_result();
}

CheckerExprResult Checker::execute(const Conditional& node) {
  visit_node(node.clause);
  visit_node(node.if_true);
  if (node.if_false) visit_node(node.if_false);
  return stmt_result();
}

CheckerExprResult Checker::execute(const Match&) {
  return stmt_result();
}

CheckerExprResult Checker::execute(const MatchCase&) {
  return stmt_result();
}

CheckerExprResult Checker::execute(const Block& node) {
  for (const auto& child_node : node.body) {
    visit_node(child_node);
  }

  return stmt_result();
}

CheckerExprResult Checker::execute(const Interrupt&) {
  return stmt_result();
}

CheckerExprResult Checker::execute(const Return&) {
  return stmt_result();
}

// check ast function
auto Checker::check(const AST& ast) noexcept -> void {
  for (const auto& node : ast.program) {
    try {
      visit_node(node);
    } catch (...) {}
  }
}
