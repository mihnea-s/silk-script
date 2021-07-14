#pragma once

#include <silk/language/package.h>
#include <silk/language/syntax_tree.h>
#include <silk/pipeline/stage.h>

namespace silk {

class TypeChecker final :
    public Stage<TypeChecker, Package, Package, st::Typing> {
private:
  auto handle(st::Node &, st::Comment &) -> st::Typing override;
  auto handle(st::Node &, st::ModuleMain &) -> st::Typing override;
  auto handle(st::Node &, st::ModuleDeclaration &) -> st::Typing override;
  auto handle(st::Node &, st::ModuleImport &) -> st::Typing override;
  auto handle(st::Node &, st::DeclarationFunction &) -> st::Typing override;
  auto handle(st::Node &, st::DeclarationEnum &) -> st::Typing override;
  auto handle(st::Node &, st::DeclarationObject &) -> st::Typing override;
  auto handle(st::Node &, st::DeclarationExternLibrary &)
    -> st::Typing override;
  auto handle(st::Node &, st::DeclarationExternFunction &)
    -> st::Typing override;
  auto handle(st::Node &, st::DeclarationMacro &) -> st::Typing override;
  auto handle(st::Node &, st::StatementEmpty &) -> st::Typing override;
  auto handle(st::Node &, st::StatementExpression &) -> st::Typing override;
  auto handle(st::Node &, st::StatementBlock &) -> st::Typing override;
  auto handle(st::Node &, st::StatementCircuit &) -> st::Typing override;
  auto handle(st::Node &, st::StatementVariable &) -> st::Typing override;
  auto handle(st::Node &, st::StatementConstant &) -> st::Typing override;
  auto handle(st::Node &, st::StatementReturn &) -> st::Typing override;
  auto handle(st::Node &, st::StatementSwitch &) -> st::Typing override;
  auto handle(st::Node &, st::StatementIterationControl &)
    -> st::Typing override;
  auto handle(st::Node &, st::StatementIf &) -> st::Typing override;
  auto handle(st::Node &, st::StatementWhile &) -> st::Typing override;
  auto handle(st::Node &, st::StatementLoop &) -> st::Typing override;
  auto handle(st::Node &, st::StatementFor &) -> st::Typing override;
  auto handle(st::Node &, st::StatementForeach &) -> st::Typing override;
  auto handle(st::Node &, st::StatementMatch &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionIdentifier &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionVoid &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionContinuation &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionBool &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionNat &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionInt &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionReal &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionRealKeyword &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionChar &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionString &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionTuple &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionUnaryOp &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionBinaryOp &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionRange &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionVector &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionArray &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionDictionary &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionAssignment &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionCall &) -> st::Typing override;
  auto handle(st::Node &, st::ExpressionLambda &) -> st::Typing override;

public:
  TypeChecker() {
  }

  ~TypeChecker() {
  }

  TypeChecker(const TypeChecker &) = delete;
  TypeChecker(TypeChecker &&)      = default;

  auto type_check(Module &) noexcept;
  auto execute(Package &&) noexcept -> Package override;
};

} // namespace silk
