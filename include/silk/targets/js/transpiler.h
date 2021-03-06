#pragma once

#include <silk/language/package.h>
#include <silk/language/syntax_tree.h>
#include <silk/pipeline/stage.h>

namespace silk {

namespace js {

using JsSource = std::string;

class Transpiler final : public Stage<Transpiler, Package, JsSource> {
private:
  auto handle(st::Node &, st::Comment &) -> void override;
  auto handle(st::Node &, st::ModuleMain &) -> void override;
  auto handle(st::Node &, st::ModuleDeclaration &) -> void override;
  auto handle(st::Node &, st::ModuleImport &) -> void override;

  auto handle(st::Node &, st::DeclarationFunction &) -> void override;
  auto handle(st::Node &, st::DeclarationEnum &) -> void override;
  auto handle(st::Node &, st::DeclarationObject &) -> void override;
  auto handle(st::Node &, st::DeclarationExternLibrary &) -> void override;
  auto handle(st::Node &, st::DeclarationExternFunction &) -> void override;
  auto handle(st::Node &, st::DeclarationMacro &) -> void override;

  auto handle(st::Node &, st::StatementEmpty &) -> void override;
  auto handle(st::Node &, st::StatementExpression &) -> void override;
  auto handle(st::Node &, st::StatementBlock &) -> void override;
  auto handle(st::Node &, st::StatementCircuit &) -> void override;
  auto handle(st::Node &, st::StatementVariable &) -> void override;
  auto handle(st::Node &, st::StatementConstant &) -> void override;
  auto handle(st::Node &, st::StatementReturn &) -> void override;
  auto handle(st::Node &, st::StatementSwitch &) -> void override;
  auto handle(st::Node &, st::StatementIterationControl &) -> void override;
  auto handle(st::Node &, st::StatementIf &) -> void override;
  auto handle(st::Node &, st::StatementWhile &) -> void override;
  auto handle(st::Node &, st::StatementLoop &) -> void override;
  auto handle(st::Node &, st::StatementFor &) -> void override;
  auto handle(st::Node &, st::StatementForeach &) -> void override;
  auto handle(st::Node &, st::StatementMatch &) -> void override;

  auto handle(st::Node &, st::ExpressionIdentifier &) -> void override;
  auto handle(st::Node &, st::ExpressionVoid &) -> void override;
  auto handle(st::Node &, st::ExpressionContinuation &) -> void override;
  auto handle(st::Node &, st::ExpressionBool &) -> void override;
  auto handle(st::Node &, st::ExpressionNat &) -> void override;
  auto handle(st::Node &, st::ExpressionInt &) -> void override;
  auto handle(st::Node &, st::ExpressionReal &) -> void override;
  auto handle(st::Node &, st::ExpressionRealKeyword &) -> void override;
  auto handle(st::Node &, st::ExpressionChar &) -> void override;
  auto handle(st::Node &, st::ExpressionString &) -> void override;
  auto handle(st::Node &, st::ExpressionTuple &) -> void override;
  auto handle(st::Node &, st::ExpressionUnaryOp &) -> void override;
  auto handle(st::Node &, st::ExpressionBinaryOp &) -> void override;
  auto handle(st::Node &, st::ExpressionRange &) -> void override;
  auto handle(st::Node &, st::ExpressionVector &) -> void override;
  auto handle(st::Node &, st::ExpressionArray &) -> void override;
  auto handle(st::Node &, st::ExpressionDictionary &) -> void override;
  auto handle(st::Node &, st::ExpressionAssignment &) -> void override;
  auto handle(st::Node &, st::ExpressionCall &) -> void override;
  auto handle(st::Node &, st::ExpressionLambda &) -> void override;

public:
  Transpiler() {
  }

  ~Transpiler() {
  }

  Transpiler(const Transpiler &) = delete;
  Transpiler(Transpiler &&)      = default;

  auto execute(Package &&) noexcept -> JsSource override;
};

} // namespace js

} // namespace silk
