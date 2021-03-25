#pragma once

#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include <silk/stages/pipeline.h>
#include <silk/syntax/tree.h>
#include <silk/tools/cli.h>

namespace silk {

class TreePrinter final : public silk::Stage<TreePrinter, Module, std::string> {
private:
  std::stringstream _output            = {};
  size_t            _indentation_level = 0;

  void indent();
  void unindent();

  void append_semi();
  void append_line();

  template <class... Args>
  void append(std::string_view frmt, Args... args) {
    _output << std::string(_indentation_level, '\t');
    _output << fmt_function(frmt, std::forward<Args>(args)...);
  }

  template <class... Args>
  void append_line(std::string_view frmt, Args... args) {
    append(frmt, std::forward<Args>(args)...);
    _output << std::endl;
  }

  void handle(st::Node &, st::Comment &) override;
  void handle(st::Node &, st::ModuleMain &) override;
  void handle(st::Node &, st::ModuleDeclaration &) override;
  void handle(st::Node &, st::ModuleImport &) override;
  void handle(st::Node &, st::DeclarationFunction &) override;
  void handle(st::Node &, st::DeclarationEnum &) override;
  void handle(st::Node &, st::DeclarationObject &) override;
  void handle(st::Node &, st::DeclarationDynamicLibrary &) override;
  void handle(st::Node &, st::DeclarationMacro &) override;
  void handle(st::Node &, st::StatementEmpty &) override;
  void handle(st::Node &, st::StatementExpression &) override;
  void handle(st::Node &, st::StatementBlock &) override;
  void handle(st::Node &, st::StatementCircuit &) override;
  void handle(st::Node &, st::StatementVariable &) override;
  void handle(st::Node &, st::StatementConstant &) override;
  void handle(st::Node &, st::StatementReturn &) override;
  void handle(st::Node &, st::StatementSwitch &) override;
  void handle(st::Node &, st::StatementIterationControl &) override;
  void handle(st::Node &, st::StatementIf &) override;
  void handle(st::Node &, st::StatementWhile &) override;
  void handle(st::Node &, st::StatementLoop &) override;
  void handle(st::Node &, st::StatementFor &) override;
  void handle(st::Node &, st::StatementForeach &) override;
  void handle(st::Node &, st::StatementMatch &) override;
  void handle(st::Node &, st::ExpressionIdentifier &) override;
  void handle(st::Node &, st::ExpressionVoid &) override;
  void handle(st::Node &, st::ExpressionContinuation &) override;
  void handle(st::Node &, st::ExpressionBool &) override;
  void handle(st::Node &, st::ExpressionNat &) override;
  void handle(st::Node &, st::ExpressionInt &) override;
  void handle(st::Node &, st::ExpressionReal &) override;
  void handle(st::Node &, st::ExpressionRealKeyword &) override;
  void handle(st::Node &, st::ExpressionChar &) override;
  void handle(st::Node &, st::ExpressionString &) override;
  void handle(st::Node &, st::ExpressionTuple &) override;
  void handle(st::Node &, st::ExpressionUnaryOp &) override;
  void handle(st::Node &, st::ExpressionBinaryOp &) override;
  void handle(st::Node &, st::ExpressionRange &) override;
  void handle(st::Node &, st::ExpressionVector &) override;
  void handle(st::Node &, st::ExpressionArray &) override;
  void handle(st::Node &, st::ExpressionDictionary &) override;
  void handle(st::Node &, st::ExpressionAssignment &) override;
  void handle(st::Node &, st::ExpressionCall &) override;
  void handle(st::Node &, st::ExpressionLambda &) override;

public:
  TreePrinter() {
  }

  ~TreePrinter() {
  }

  TreePrinter(const TreePrinter &) = delete;
  TreePrinter(TreePrinter &&)      = default;

  auto execute(Module &&) noexcept -> std::string override;
};

} // namespace silk