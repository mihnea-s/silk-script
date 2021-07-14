#include <silk/targets/wasm/compiler.h>

#include <silk/language/syntax_tree.h>

namespace silk {

namespace wasm {

auto Compiler::handle(st::Node &, st::Comment &) -> void {
}

auto Compiler::handle(st::Node &, st::ModuleMain &) -> void {
}

auto Compiler::handle(st::Node &, st::ModuleDeclaration &) -> void {
}

auto Compiler::handle(st::Node &, st::ModuleImport &) -> void {
}

auto Compiler::handle(st::Node &, st::DeclarationFunction &) -> void {
}

auto Compiler::handle(st::Node &, st::DeclarationEnum &) -> void {
}

auto Compiler::handle(st::Node &, st::DeclarationObject &) -> void {
}

auto Compiler::handle(st::Node &, st::DeclarationExternLibrary &)
  -> void {
}

auto Compiler::handle(st::Node &, st::DeclarationExternFunction &)
  -> void {
}

auto Compiler::handle(st::Node &, st::DeclarationMacro &) -> void {
}

auto Compiler::handle(st::Node &, st::StatementEmpty &) -> void {
}

auto Compiler::handle(st::Node &, st::StatementExpression &) -> void {
}

auto Compiler::handle(st::Node &, st::StatementBlock &) -> void {
}

auto Compiler::handle(st::Node &, st::StatementCircuit &) -> void {
}

auto Compiler::handle(st::Node &, st::StatementVariable &) -> void {
}

auto Compiler::handle(st::Node &, st::StatementConstant &) -> void {
}

auto Compiler::handle(st::Node &, st::StatementReturn &) -> void {
}

auto Compiler::handle(st::Node &, st::StatementSwitch &) -> void {
}

auto Compiler::handle(st::Node &, st::StatementIterationControl &)
  -> void {
}

auto Compiler::handle(st::Node &, st::StatementIf &) -> void {
}

auto Compiler::handle(st::Node &, st::StatementWhile &) -> void {
}

auto Compiler::handle(st::Node &, st::StatementLoop &) -> void {
}

auto Compiler::handle(st::Node &, st::StatementFor &) -> void {
}

auto Compiler::handle(st::Node &, st::StatementForeach &) -> void {
}

auto Compiler::handle(st::Node &, st::StatementMatch &) -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionIdentifier &) -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionVoid &) -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionContinuation &)
  -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionBool &) -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionNat &) -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionInt &) -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionReal &) -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionRealKeyword &) -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionChar &) -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionString &) -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionTuple &) -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionUnaryOp &) -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionBinaryOp &) -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionRange &) -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionVector &) -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionArray &) -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionDictionary &) -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionAssignment &) -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionCall &) -> void {
}

auto Compiler::handle(st::Node &, st::ExpressionLambda &) -> void {
}

auto Compiler::execute(Package &&) noexcept -> void {
}

} // namespace wasm

} // namespace silk
