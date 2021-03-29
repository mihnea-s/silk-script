#include <silk/pipeline/optimizer.h>

namespace silk {

auto Optimizer::handle(st::Node &node, st::Comment &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ModuleMain &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ModuleDeclaration &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ModuleImport &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::DeclarationFunction &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::DeclarationEnum &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::DeclarationObject &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::DeclarationDynamicLibrary &data)
  -> void {
}

auto Optimizer::handle(st::Node &node, st::DeclarationMacro &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::StatementEmpty &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::StatementExpression &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::StatementBlock &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::StatementCircuit &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::StatementVariable &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::StatementConstant &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::StatementReturn &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::StatementSwitch &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::StatementIterationControl &data)
  -> void {
}

auto Optimizer::handle(st::Node &node, st::StatementIf &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::StatementWhile &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::StatementLoop &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::StatementFor &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::StatementForeach &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::StatementMatch &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionIdentifier &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionVoid &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionContinuation &data)
  -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionBool &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionNat &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionInt &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionReal &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionRealKeyword &data)
  -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionChar &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionString &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionTuple &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionUnaryOp &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionBinaryOp &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionRange &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionVector &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionArray &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionDictionary &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionAssignment &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionCall &data) -> void {
}

auto Optimizer::handle(st::Node &node, st::ExpressionLambda &data) -> void {
}

auto Optimizer::execute(Module &&mod) noexcept -> Module {
  return std::move(mod);
}

} // namespace silk