#pragma once

#include <iomanip>

#include <silk/language/syntax_tree.h>
#include <silk/pipeline/stage.h>
#include <silk/utility/cli.h>

namespace silk {

class JsonSerializer final :
    public Stage<JsonSerializer, Package, std::string> {
private:
  std::stringstream _output = {};

  void obj_beg();
  void obj_end();
  void arr_beg();
  void arr_end();

  void serialize(bool);
  void serialize(wchar_t);
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
  void serialize(st::ExpressionAssignment::Kind);
  void serialize(st::ExpressionUnaryOp::Kind);
  void serialize(st::ExpressionBinaryOp::Kind);

  void serialize(Module&);

  template <class T>
  void keyval(std::string_view key, T &&value) {
    _output << std::quoted(key) << ':';
    serialize(value);
    _output << ',';
  }

  void keyval(std::string_view key, const char *value) {
    _output << std::quoted(key) << ':' << std::quoted(value) << ',';
  }

  template <class T>
  void keyval(std::string_view key, std::vector<T> &array) {
    _output << std::quoted(key) << ':';
    arr_beg();

    for (auto &&elem : array) {
      serialize(elem);
      _output << ',';
    }

    arr_end();
    _output << ',';
  }

  template <class K, class V>
  void keyval(std::string_view key, std::vector<std::pair<K, V>> &map) {
    _output << std::quoted(key) << ':';
    obj_beg();

    for (auto &&[key, val] : map) {
      keyval(key, val);
    }

    obj_end();
    _output << ',';
  }

  void keyval(
    std::string_view key, std::vector<std::pair<st::Node, st::Node>> &map) {
    _output << std::quoted(key) << ':';
    arr_beg();

    for (auto &&[key, val] : map) {
      obj_beg();
      keyval("key", key);
      keyval("value", val);
      obj_end();
    }

    arr_end();
    _output << ',';
  }

  void handle(st::Node &, st::Comment &) override;
  void handle(st::Node &, st::ModuleMain &) override;
  void handle(st::Node &, st::ModuleDeclaration &) override;
  void handle(st::Node &, st::ModuleImport &) override;
  void handle(st::Node &, st::DeclarationFunction &) override;
  void handle(st::Node &, st::DeclarationEnum &) override;
  void handle(st::Node &, st::DeclarationObject &) override;
  void handle(st::Node &, st::DeclarationExternLibrary &) override;
  void handle(st::Node &, st::DeclarationExternFunction &) override;
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

  auto execute(Package &&) noexcept -> std::string override;
};

} // namespace silk
