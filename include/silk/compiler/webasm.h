#pragma once

#include "silk/syntax/tree.h"
#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <stack>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <moth/program.h>
#include <moth/value.h>
#include <moth/vm.h>

#include <silk/stages/pipeline.h>

namespace silk {

class CompilerWebASM final : public Stage<CompilerWebASM, Package, void> {
private:
  auto handle(st::Node &, st::Comment &) -> void override;
  auto handle(st::Node &, st::ModuleMain &) -> void override;
  auto handle(st::Node &, st::ModuleDeclaration &) -> void override;
  auto handle(st::Node &, st::ModuleImport &) -> void override;

  auto handle(st::Node &, st::DeclarationFunction &) -> void override;
  auto handle(st::Node &, st::DeclarationEnum &) -> void override;
  auto handle(st::Node &, st::DeclarationObject &) -> void override;
  auto handle(st::Node &, st::DeclarationDynamicLibrary &) -> void override;
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
  CompilerWebASM() {
  }

  ~CompilerWebASM() {
  }

  CompilerWebASM(const CompilerWebASM &) = delete;
  CompilerWebASM(CompilerWebASM &&)      = default;

  auto execute(Package &&) noexcept -> void override;
};

} // namespace silk
