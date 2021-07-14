#include <silk/pipeline/optimizer.h>

#include <cmath>
#include <optional>
#include <variant>

#include <silk/language/syntax_tree.h>

namespace silk {

auto Optimizer::is_const_expr(std::unique_ptr<st::Node> &node) const -> bool {
  return st::node_contains<
    st::ExpressionBool,
    st::ExpressionNat,
    st::ExpressionInt,
    st::ExpressionReal,
    st::ExpressionRealKeyword,
    st::ExpressionChar,
    st::ExpressionString>(node);
}

auto Optimizer::nat_value(st::Node &node) const
  -> std::optional<std::uint64_t> {
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

auto Optimizer::real_value(st::Node &node) const -> std::optional<double> {

  if (auto v = std::get_if<st::ExpressionReal>(&node.data); v) {
    return v->value;
  }

  if (auto v = std::get_if<st::ExpressionRealKeyword>(&node.data); v) {
    switch (v->kind) {
      case st::ExpressionRealKeyword::EULER: return std::exp(1.0);

      case st::ExpressionRealKeyword::PI: return M_PI;

      case st::ExpressionRealKeyword::TAU: return 2.0 * M_PI;
    }
  }

  return std::nullopt;
}

auto Optimizer::handle(st::Node &, st::Comment &) -> void {
  // TODO
}

auto Optimizer::handle(st::Node &, st::ModuleMain &) -> void {
  // TODO
}

auto Optimizer::handle(st::Node &, st::ModuleDeclaration &) -> void {
  // TODO
}

auto Optimizer::handle(st::Node &, st::ModuleImport &) -> void {
  // TODO
}

auto Optimizer::handle(st::Node &, st::DeclarationFunction &data) -> void {
  handle_node(data.child);
}

auto Optimizer::handle(st::Node &, st::DeclarationEnum &) -> void {
  // TODO
}

auto Optimizer::handle(st::Node &, st::DeclarationObject &data) -> void {
  handle_nodes(data.children);
}

auto Optimizer::handle(st::Node &, st::DeclarationExternLibrary &data) -> void {
  handle_nodes(data.children);
}

auto Optimizer::handle(st::Node &, st::DeclarationExternFunction &) -> void {
  // TODO
}

auto Optimizer::handle(st::Node &, st::DeclarationMacro &) -> void {
  // TODO
}

auto Optimizer::handle(st::Node &, st::StatementEmpty &) -> void {
  // Noting to optimize.
}

auto Optimizer::handle(st::Node &, st::StatementExpression &data) -> void {
  handle_node(data.child);
}

auto Optimizer::handle(st::Node &, st::StatementBlock &data) -> void {
  handle_nodes(data.children);
}

auto Optimizer::handle(st::Node &, st::StatementCircuit &data) -> void {
  handle_node(data.default_switch);

  for (auto &[_, node] : data.children) {
    handle_node(node);
  }
}

auto Optimizer::handle(st::Node &, st::StatementVariable &data) -> void {
  handle_node(data.child);
}

auto Optimizer::handle(st::Node &node, st::StatementConstant &data) -> void {
  handle_node(data.child);

  if (!is_const_expr(data.child)) {
    report("expression is not constant", node.location);
  }
}

auto Optimizer::handle(st::Node &, st::StatementReturn &data) -> void {
  handle_node(data.child);
}

auto Optimizer::handle(st::Node &, st::StatementSwitch &) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &, st::StatementIterationControl &) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &, st::StatementIf &data) -> void {
  handle_node(data.condition);

  handle_node(data.consequence);
  handle_node(data.alternative);
}

auto Optimizer::handle(st::Node &, st::StatementWhile &data) -> void {
  handle_node(data.condition);
  handle_node(data.child);
}

auto Optimizer::handle(st::Node &, st::StatementLoop &data) -> void {
  handle_node(data.child);
}

auto Optimizer::handle(st::Node &, st::StatementFor &data) -> void {
  handle_node(data.initial);
  handle_node(data.condition);
  handle_node(data.increment);
  handle_node(data.child);
}

auto Optimizer::handle(st::Node &, st::StatementForeach &data) -> void {
  handle_node(data.collection);
  handle_node(data.child);
}

auto Optimizer::handle(st::Node &, st::StatementMatch &) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &, st::ExpressionIdentifier &) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &, st::ExpressionVoid &) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &, st::ExpressionContinuation &) -> void {
  //  TODO
}

auto Optimizer::handle(st::Node &, st::ExpressionBool &) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &, st::ExpressionNat &) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &, st::ExpressionInt &) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &, st::ExpressionReal &) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &, st::ExpressionRealKeyword &) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &, st::ExpressionChar &) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &, st::ExpressionString &) -> void {
  // Nothing to optimize.
}

auto Optimizer::handle(st::Node &, st::ExpressionTuple &data) -> void {
  handle_nodes(data.children);
}

auto Optimizer::handle(st::Node &, st::ExpressionUnaryOp &data) -> void {
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

auto Optimizer::handle(st::Node &, st::ExpressionRange &data) -> void {
  handle_node(data.left);
  handle_node(data.right);
}

auto Optimizer::handle(st::Node &, st::ExpressionVector &data) -> void {
  handle_nodes(data.children);
}

auto Optimizer::handle(st::Node &, st::ExpressionArray &data) -> void {
  handle_nodes(data.children);
}

auto Optimizer::handle(st::Node &, st::ExpressionDictionary &data) -> void {
  for (auto &[key, value] : data.children) {
    handle_node(key);
    handle_node(value);
  }
}

auto Optimizer::handle(st::Node &, st::ExpressionAssignment &data) -> void {
  handle_node(data.assignee);
  handle_node(data.child);
}

auto Optimizer::handle(st::Node &, st::ExpressionCall &data) -> void {
  handle_node(data.callee);

  for (auto &arg : data.children) {
    handle_node(arg);
  }
}

auto Optimizer::handle(st::Node &, st::ExpressionLambda &data) -> void {
  handle_node(data.child);
}

auto Optimizer::optimize(Module &mod) noexcept -> void {
  for (auto &node : mod.tree) {
    handle_node(node);
  }
}

auto Optimizer::execute(Package &&pkg) noexcept -> Package {
  optimize(pkg.main);

  for (auto &[_, mod] : pkg.modules) {
    optimize(mod);
  }

  return std::move(pkg);
}

} // namespace silk
