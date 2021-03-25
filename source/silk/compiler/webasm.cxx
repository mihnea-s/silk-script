#include "silk/syntax/tree.h"
#include <silk/compiler/webasm.h>

namespace silk {

auto CompilerWebASM::handle(st::Node &node, st::Comment &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ModuleMain &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ModuleDeclaration &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ModuleImport &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::DeclarationFunction &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::DeclarationEnum &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::DeclarationObject &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::DeclarationDynamicLibrary &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::DeclarationMacro &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::StatementEmpty &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::StatementExpression &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::StatementBlock &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::StatementCircuit &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::StatementVariable &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::StatementConstant &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::StatementReturn &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::StatementSwitch &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::StatementIterationControl &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::StatementIf &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::StatementWhile &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::StatementLoop &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::StatementFor &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::StatementForeach &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::StatementMatch &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionIdentifier &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionVoid &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionContinuation &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionBool &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionNat &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionInt &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionReal &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionRealKeyword &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionChar &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionString &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionTuple &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionUnaryOp &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionBinaryOp &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionRange &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionVector &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionArray &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionDictionary &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionAssignment &data)
  -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionCall &data) -> void {
}

auto CompilerWebASM::handle(st::Node &node, st::ExpressionLambda &data)
  -> void {
}

auto CompilerWebASM::execute(Package &&pkg) noexcept -> void {
}

} // namespace silk