#include <silk/pipeline/optimizer.h>

#include <cmath>
#include <optional>
#include <variant>

#include <silk/language/syntax_tree.h>

namespace silk {

auto Optimizer::is_const_expr(std::unique_ptr<st::Node>& node) const -> bool {
  return st::node_contains<
    st::ExpressionBool, st::ExpressionNat, st::ExpressionInt, 
    st::ExpressionReal, st::ExpressionRealKeyword, st::ExpressionChar,
    st::ExpressionString>(node);
}

auto Optimizer::nat_value(st::Node &node) const -> std::optional<std::uint64_t> {
  if (auto v = std::get_if<st::ExpressionNat>(&node.data); v) {
    return v->value;
  }

  return std::nullopt;
}

auto Optimizer::int_value(st::Node &node) const -> std::optional<std::int64_t> {
  if (auto v = std::get_if<st::ExpressionInt>(&node.data); v) {
    return v->value;
  }

  return std::nullopt;
}

auto Optimizer::real_value(st::Node & node) const -> std::optional<double> {

  if (auto v = std::get_if<st::ExpressionReal>(&node.data); v) {
    return v->value;
  }

  if (auto v = std::get_if<st::ExpressionRealKeyword>(&node.data); v) {
    switch (v->kind) {
      case st::ExpressionRealKeyword::EULER:
        return std::exp(1.0);

      case st::ExpressionRealKeyword::PI:
        return M_PI;

      case st::ExpressionRealKeyword::TAU:
        return 2.0 * M_PI;
    }
  }

  return std::nullopt;
}

auto Optimizer::handle(st::Node &node, st::Comment &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ModuleMain &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ModuleDeclaration &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ModuleImport &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::DeclarationFunction &data) -> void {
  handle_node(data.child);
}

auto Optimizer::handle(st::Node &node, st::DeclarationEnum &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::DeclarationObject &data) -> void {
  handle_nodes(data.children);
}

auto Optimizer::handle(st::Node &node, st::DeclarationExternLibrary &data)
  -> void {
  handle_nodes(data.children);
}

auto Optimizer::handle(st::Node &node, st::DeclarationExternFunction &data)
  -> void {
}

auto Optimizer::handle(st::Node &node, st::DeclarationMacro &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::StatementEmpty &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::StatementExpression &data) -> void {
  handle_node(data.child);
}

auto Optimizer::handle(st::Node &node, st::StatementBlock &data) -> void {
  handle_nodes(data.children);
}

auto Optimizer::handle(st::Node &node, st::StatementCircuit &data) -> void {
  handle_node(data.default_switch);

  for (auto &[_, node] : data.children) {
    handle_node(node);
  }
}

auto Optimizer::handle(st::Node &node, st::StatementVariable &data) -> void {
  handle_node(data.child);
}

auto Optimizer::handle(st::Node &node, st::StatementConstant &data) -> void {
  handle_node(data.child);

  if (!is_const_expr(data.child)) {
    report("expression is not constant", node.location);
  }
}

auto Optimizer::handle(st::Node &node, st::StatementReturn &data) -> void {
  handle_node(data.child);
}

auto Optimizer::handle(st::Node &node, st::StatementSwitch &data) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &node, st::StatementIterationControl &data)
  -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &node, st::StatementIf &data) -> void {
  handle_node(data.condition);

  handle_node(data.consequence);
  handle_node(data.alternative);
}

auto Optimizer::handle(st::Node &node, st::StatementWhile &data) -> void {
  handle_node(data.condition);
  handle_node(data.child);
}

auto Optimizer::handle(st::Node &node, st::StatementLoop &data) -> void {
  handle_node(data.child);
}

auto Optimizer::handle(st::Node &node, st::StatementFor &data) -> void {
  handle_node(data.initial);
  handle_node(data.condition);
  handle_node(data.increment);
  handle_node(data.child);
}

auto Optimizer::handle(st::Node &node, st::StatementForeach &data) -> void {
  handle_node(data.collection);
  handle_node(data.child);
}

auto Optimizer::handle(st::Node &node, st::StatementMatch &data) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &node, st::ExpressionIdentifier &data) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &node, st::ExpressionVoid &data) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &node, st::ExpressionContinuation &data)
  -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionBool &data) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &node, st::ExpressionNat &data) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &node, st::ExpressionInt &data) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &node, st::ExpressionReal &data) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &node, st::ExpressionRealKeyword &data)
  -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &node, st::ExpressionChar &data) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &node, st::ExpressionString &data) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &node, st::ExpressionTuple &data) -> void {
  handle_nodes(data.children);
}

auto Optimizer::handle(st::Node &node, st::ExpressionUnaryOp &data) -> void {
  handle_node(data.child);
}

auto Optimizer::handle(st::Node &node, st::ExpressionBinaryOp &data) -> void {
  // Optimize children first
  handle_node(data.left);
  handle_node(data.right);

  // Check for number addition
  if (auto left = nat_value(*data.left); left) {
    if (auto right = nat_value(*data.right); right) {
      node.data = make_const_bin_expr<st::ExpressionNat>(
        data.kind, left.value(), right.value());
      return;
    }
    
    if (auto right = int_value(*data.right); right) {
      node.data = make_const_bin_expr<st::ExpressionInt>(
        data.kind, left.value(), right.value());
      return;
    }

    if (auto right = real_value(*data.right); right) {
      node.data = make_const_bin_expr<st::ExpressionReal>(
        data.kind, left.value(), right.value());
      return;
    }
  }

  if (auto left = int_value(*data.left); left) {
    if (auto right = nat_value(*data.right); right) {
      node.data = make_const_bin_expr<st::ExpressionInt>(
        data.kind, left.value(), right.value());
      return;
    }
    
    if (auto right = int_value(*data.right); right) {
      node.data = make_const_bin_expr<st::ExpressionInt>(
        data.kind, left.value(), right.value());
      return;
    }

    if (auto right = real_value(*data.right); right) {
      node.data = make_const_bin_expr<st::ExpressionReal>(
        data.kind, left.value(), right.value());
      return;
    }
  }

  if (auto left = real_value(*data.left); left) {
    if (auto right = nat_value(*data.right); right) {
      node.data = make_const_bin_expr<st::ExpressionReal>(
        data.kind, left.value(), right.value());
      return;
    }
    
    if (auto right = int_value(*data.right); right) {
      node.data = make_const_bin_expr<st::ExpressionReal>(
        data.kind, left.value(), right.value());
      return;
    }

    if (auto right = real_value(*data.right); right) {
      node.data = make_const_bin_expr<st::ExpressionReal>(
        data.kind, left.value(), right.value());
      return;
    }
  }
}

auto Optimizer::handle(st::Node &node, st::ExpressionRange &data) -> void {
  handle_node(data.left);
  handle_node(data.right);
}

auto Optimizer::handle(st::Node &node, st::ExpressionVector &data) -> void {
  handle_nodes(data.children);
}

auto Optimizer::handle(st::Node &node, st::ExpressionArray &data) -> void {
  handle_nodes(data.children);
}

auto Optimizer::handle(st::Node &node, st::ExpressionDictionary &data) -> void {
  for (auto &[key, value] : data.children) {
    handle_node(key);
    handle_node(value);
  }
}

auto Optimizer::handle(st::Node &node, st::ExpressionAssignment &data) -> void {
  handle_node(data.assignee);
  handle_node(data.child);
}

auto Optimizer::handle(st::Node &node, st::ExpressionCall &data) -> void {
  handle_node(data.callee);

  for (auto& arg : data.children) {
    handle_node(arg);
  }
}

auto Optimizer::handle(st::Node &node, st::ExpressionLambda &data) -> void {
  handle_node(data.child);
}

auto Optimizer::execute(Module &&mod) noexcept -> Module {
  for (auto &node : mod.tree) {
    handle_node(node);
  }

  return std::move(mod);
}

} // namespace silk