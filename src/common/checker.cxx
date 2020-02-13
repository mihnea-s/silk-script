#include <silk/common/checker.h>
#include <silk/common/error.h>

auto Checker::declare_var(const std::string& name, SilkType type) -> void {
  _variables.insert_or_assign(name, type);
}

auto Checker::variable_type(const std::string& name) -> SilkType {
  if (_variables.find(name) == _variables.end()) { return SilkType::DYNAMIC; }
  return _variables[name];
}

SilkType Checker::evaluate(const Unary& node) {
  auto operand = visit_node(node.operand);

  switch (node.operation) {

    case TokenType::sym_bang: return SilkType::BOOLEAN;

    case TokenType::sym_tilde: {
      if (!of_type_or_dyn(operand, SilkType::INTEGER)) {
        throw report_error(node.location, "invalid operator to operator tilde");
      }

      return SilkType::INTEGER;
    }

    case TokenType::sym_minus: {
      if (!of_type_or_dyn(operand, SilkType::REAL, SilkType::INTEGER)) {
        throw report_error(node.location, "invalid operator to operator minus");
      }

      return operand;
    };

    default: return SilkType::DYNAMIC;
  }
}

SilkType Checker::evaluate(const Binary&) {
  return SilkType::DYNAMIC;
}

SilkType Checker::evaluate(const IntLiteral&) {
  return SilkType::INTEGER;
}

SilkType Checker::evaluate(const RealLiteral&) {
  return SilkType::REAL;
}

SilkType Checker::evaluate(const BoolLiteral&) {
  return SilkType::BOOLEAN;
}

SilkType Checker::evaluate(const StringLiteral&) {
  return SilkType::STRING;
}

SilkType Checker::evaluate(const Vid&) {
  return SilkType::DYNAMIC;
}

SilkType Checker::evaluate(const Constant& node) {
  return SilkType::REAL;
}

SilkType Checker::evaluate(const Lambda&) {
  return SilkType::CALLABLE;
}

SilkType Checker::evaluate(const Identifier& node) {
  return variable_type(node.identifier);
}

SilkType Checker::evaluate(const Assignment& node) {
  auto assign_type = visit_node(node.assignment);
  auto target_type = visit_node(node.target);

  if (!of_type_or_dyn(target_type, assign_type)) {
    throw report_error(
      node.location,
      "type mismatch: variable of type `{}` was assigned `{}`",
      str_from_type(target_type),
      str_from_type(assign_type));
  }

  return assign_type;
}

SilkType Checker::evaluate(const Grouping& node) {
  return visit_node(node.inner);
}

SilkType Checker::evaluate(const Call& node) {
  auto callee = visit_node(node.target);

  if (!of_type_or_dyn(callee, SilkType::CALLABLE)) {
    throw report_error(node.location, "tried calling type `{}`", callee);
  }

  return SilkType::DYNAMIC;
}

SilkType Checker::evaluate(const Get& node) {
  auto target = visit_node(node.target);

  if (!of_type_or_dyn(target, SilkType::INSTANCE)) {
    throw report_error(node.location, "tried accessing type `{}`", target);
  }

  return SilkType::DYNAMIC;
}

SilkType Checker::execute(const Empty&) {
  return SilkType::NONE;
}

SilkType Checker::execute(const Package&) {
  return SilkType::NONE;
}

SilkType Checker::execute(const ExprStmt&) {
  return SilkType::NONE;
}

SilkType Checker::execute(const Variable& node) {
  auto var_type  = type_from_str(node.type);
  auto init_type = visit_node(node.initializer);

  declare_var(node.name, var_type);

  if (!of_type_or_dyn(init_type, var_type)) {
    throw report_error(
      node.location,
      "type mismatch: `{}` of type `{}` was initialized with `{}`",
      node.name,
      str_from_type(var_type),
      str_from_type(init_type));
  }

  return SilkType::NONE;
}

SilkType Checker::execute(const Function&) {
  return SilkType::NONE;
}

SilkType Checker::execute(const Struct&) {
  return SilkType::NONE;
}

SilkType Checker::execute(const Loop& node) {
  visit_node(node.clause);
  visit_node(node.body);
  return SilkType::NONE;
}

SilkType Checker::execute(const Conditional& node) {
  visit_node(node.clause);
  visit_node(node.if_true);
  visit_node(node.if_false);
  return SilkType::NONE;
}

SilkType Checker::execute(const Block& node) {
  for (const auto& child_node : node.body) {
    visit_node(child_node);
  }

  return SilkType::NONE;
}

SilkType Checker::execute(const Interrupt&) {
  return SilkType::NONE;
}

// check ast function
auto Checker::check(const AST& ast) noexcept -> void {
  for (const auto& node : ast.program) {
    visit_node(node);
  }
}
