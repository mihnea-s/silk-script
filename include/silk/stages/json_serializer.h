#pragma once

#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include <silk/stages/pipeline.h>
#include <silk/syntax/tree.h>
#include <silk/tools/cli.h>

namespace silk {

class JsonSerializer final : public Stage<JsonSerializer, Module, std::string> {
private:
  std::stringstream _output = {};

  void obj_beg();
  void obj_end();

  void serialize(bool);
  void serialize(char32_t);
  void serialize(const char *);
  void serialize(std::string_view);

  void serialize(double);
  void serialize(std::uint64_t);
  void serialize(std::int64_t);

  void serialize(st::Node &);
  void serialize(std::nullptr_t);
  void serialize(std::unique_ptr<st::Node> &);

  void serialize(st::StatementVariable::Kind);
  void serialize(st::StatementIterationControl::Kind);
  void serialize(st::StatementForeach::IterKind);
  void serialize(st::ExpressionRealKeyword::Kind);
  void serialize(st::ExpressionUnaryOp::Kind);
  void serialize(st::ExpressionBinaryOp::Kind);

  template <class T>
  void keyval(std::string_view key, T &&value) {
    _output << '"' << key << '"' << ':';
    serialize(value);
    _output << ',';
  }

  void keyval(std::string_view key, const char *value) {
    _output << '"' << key << "\":\"" << value << "\",";
  }

  template <class T>
  void keyval(std::string_view key, std::vector<T> &array) {
    _output << '"' << key << '"';
    _output << ":[";

    for (auto &&elem : array) {
      serialize(elem);
      _output << ',';
    }

    _output << "],";
  }

  template <class K, class V>
  void keyval(std::string_view key, std::vector<std::pair<K, V>> &map) {
    _output << '"' << key << '"' << ':';
    obj_beg();

    for (auto &&[key, val] : map) {
      keyval(key, val);
    }

    obj_end();
    _output << ',';
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
  JsonSerializer() {
  }

  ~JsonSerializer() {
  }

  JsonSerializer(const JsonSerializer &) = delete;
  JsonSerializer(JsonSerializer &&)      = default;

  auto execute(Module &&) noexcept -> std::string override;
};

} // namespace silk