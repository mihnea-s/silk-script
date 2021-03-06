#include <algorithm>
#include <functional>
#include <iterator>
#include <silk/pipeline/json_serializer.h>

#include <iomanip>
#include <string_view>

#include <silk/language/syntax_tree.h>
#include <silk/utility/cli.h>
#include <vector>

namespace silk {

void JsonSerializer::obj_beg() {
  _output << '{';
}

void JsonSerializer::obj_end() {
  _output << '}';
}

void JsonSerializer::arr_beg() {
  _output << '[';
}

void JsonSerializer::arr_end() {
  _output << ']';
}

void JsonSerializer::serialize(double value) {
  _output << value;
}

void JsonSerializer::serialize(std::uint64_t value) {
  _output << value;
}

void JsonSerializer::serialize(std::int64_t value) {
  _output << value;
}

void JsonSerializer::serialize(bool value) {
  _output << (value ? "true" : "false");
}

void JsonSerializer::serialize(wchar_t value) {
  auto u32_string = std::wstring{value};
  auto converter  = std::wstring_convert<std::codecvt_utf8<wchar_t>>{};
  _output << std::quoted(converter.to_bytes(u32_string));
}

void JsonSerializer::serialize(const char *value) {
  _output << std::quoted(value);
}

void JsonSerializer::serialize(std::string_view value) {
  _output << std::quoted(value);
}

void JsonSerializer::serialize(st::Node &node) {
  handle_node(node);
}

void JsonSerializer::serialize(std::nullptr_t) {
  _output << "null";
}

void JsonSerializer::serialize(std::unique_ptr<st::Node> &node) {
  if (node) {
    handle_node(*node);
  } else {
    serialize(nullptr);
  }
}

void JsonSerializer::serialize(st::StatementVariable::Kind kind) {
  switch (kind) {
    case st::StatementVariable::LET: serialize("let"); break;
    case st::StatementVariable::DEF: serialize("def"); break;
  }
}

void JsonSerializer::serialize(st::StatementIterationControl::Kind kind) {
  switch (kind) {
    case st::StatementIterationControl::BREAK: serialize("break"); break;
    case st::StatementIterationControl::CONT: serialize("continue"); break;
  }
}

void JsonSerializer::serialize(st::StatementForeach::IterKind kind) {
  switch (kind) {
    case st::StatementForeach::LET: serialize("let"); break;
    case st::StatementForeach::DEF: serialize("def"); break;
  }
}

void JsonSerializer::serialize(st::ExpressionRealKeyword::Kind kind) {
  switch (kind) {
    case st::ExpressionRealKeyword::PI: serialize("pi"); break;
    case st::ExpressionRealKeyword::TAU: serialize("tau"); break;
    case st::ExpressionRealKeyword::EULER: serialize("euler"); break;
  }
}

void JsonSerializer::serialize(st::ExpressionAssignment::Kind kind) {
  switch (kind) {
    case st::ExpressionAssignment::ASSIGN: serialize("="); break;
    case st::ExpressionAssignment::ADD: serialize("+="); break;
    case st::ExpressionAssignment::SUB: serialize("-="); break;
    case st::ExpressionAssignment::DIV: serialize("/="); break;
    case st::ExpressionAssignment::RDIV: serialize("//="); break;
    case st::ExpressionAssignment::MUL: serialize("*="); break;
    case st::ExpressionAssignment::POW: serialize("**="); break;
  }
}

void JsonSerializer::serialize(st::ExpressionUnaryOp::Kind kind) {
  switch (kind) {
    case st::ExpressionUnaryOp::NOT: serialize("not"); break;
    case st::ExpressionUnaryOp::NEG: serialize("-"); break;
  }
}

void JsonSerializer::serialize(st::ExpressionBinaryOp::Kind kind) {
  switch (kind) {
    case st::ExpressionBinaryOp::AND: serialize("and"); break;
    case st::ExpressionBinaryOp::OR: serialize("or"); break;

    case st::ExpressionBinaryOp::EQUAL: serialize("=="); break;
    case st::ExpressionBinaryOp::NOTEQ: serialize("!="); break;
    case st::ExpressionBinaryOp::LESS: serialize("<"); break;
    case st::ExpressionBinaryOp::LESSEQ: serialize("<="); break;
    case st::ExpressionBinaryOp::GREATER: serialize(">"); break;
    case st::ExpressionBinaryOp::GREATEREQ: serialize(">="); break;

    case st::ExpressionBinaryOp::ADD: serialize("+"); break;
    case st::ExpressionBinaryOp::SUB: serialize("-"); break;
    case st::ExpressionBinaryOp::MUL: serialize("*"); break;
    case st::ExpressionBinaryOp::POW: serialize("**"); break;
    case st::ExpressionBinaryOp::DIV: serialize("/"); break;
    case st::ExpressionBinaryOp::RDIV: serialize("//"); break;
    case st::ExpressionBinaryOp::MOD: serialize("%"); break;

    case st::ExpressionBinaryOp::INDEX: serialize("."); break;
    case st::ExpressionBinaryOp::APPEND: serialize("~"); break;
    case st::ExpressionBinaryOp::MERGE: serialize("|"); break;
  }
}

void JsonSerializer::serialize(Module &mod) {
  obj_beg();
  keyval("type", "module");
  keyval("path", mod.path);
  keyval("tree", mod.tree);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::Comment &data) {
  obj_beg();
  keyval("type", "comment");
  keyval("text", data.text);
  keyval("placement", "before");
  keyval("child", data.child);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ModuleMain &) {
  obj_beg();
  keyval("type", "module");
  keyval("data", "main");
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ModuleDeclaration &data) {
  obj_beg();
  keyval("type", "module");
  keyval("data", "declaration");
  keyval("path", data.path);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ModuleImport &data) {
  obj_beg();
  keyval("type", "module");
  keyval("data", "import");
  keyval("path", data.name);

  if (data.imports.empty()) {
    keyval("imports", nullptr);
  } else {
    keyval("imports", data.imports);
  }

  obj_end();
}

void JsonSerializer::handle(st::Node &, st::DeclarationFunction &data) {
  obj_beg();
  keyval("type", "declaration");
  keyval("data", "function");
  keyval("name", data.name);
  keyval("lambda", data.child);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::DeclarationEnum &data) {
  obj_beg();
  keyval("type", "declaration");
  keyval("data", "enum");
  keyval("name", data.name);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::DeclarationObject &data) {
  obj_beg();
  keyval("type", "declaration");
  keyval("data", "obj");
  keyval("name", data.name);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::DeclarationExternLibrary &data) {
  obj_beg();
  keyval("type", "declaration");
  keyval("data", "externlibrary");
  keyval("name", data.name);
  keyval("children", data.children);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::DeclarationExternFunction &data) {
  obj_beg();
  keyval("type", "declaration");
  keyval("data", "externfunction");
  keyval("name", data.name);
  keyval("params", data.params);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::DeclarationMacro &data) {
  obj_beg();
  keyval("type", "declaration");
  keyval("data", "macro");
  keyval("name", data.name);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::StatementEmpty &) {
  obj_beg();
  keyval("type", "statement");
  keyval("data", "empty");
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::StatementExpression &data) {
  obj_beg();
  keyval("type", "statement");
  keyval("data", "expression");
  keyval("expr", data.child);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::StatementBlock &data) {
  obj_beg();
  keyval("type", "statement");
  keyval("data", "block");
  keyval("children", data.children);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::StatementCircuit &data) {
  obj_beg();
  keyval("type", "statement");
  keyval("data", "circuit");
  keyval("default_switch", data.default_switch);
  keyval("children", data.children);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::StatementVariable &data) {
  obj_beg();
  keyval("type", "statement");
  keyval("data", "variable");
  keyval("kind", data.kind);
  keyval("name", data.name);
  keyval("child", data.child);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::StatementConstant &data) {
  obj_beg();
  keyval("type", "statement");
  keyval("data", "constant");
  keyval("name", data.name);
  keyval("child", data.child);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::StatementReturn &data) {
  obj_beg();
  keyval("type", "statement");
  keyval("data", "return");
  keyval("continuation", data.continuation);
  keyval("child", data.child);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::StatementSwitch &data) {
  obj_beg();
  keyval("type", "statement");
  keyval("data", "switch");
  keyval("label", data.label);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::StatementIterationControl &data) {
  obj_beg();
  keyval("type", "statement");
  keyval("data", "iterationcontrol");
  keyval("kind", data.kind);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::StatementIf &data) {
  obj_beg();
  keyval("type", "statement");
  keyval("data", "if");
  keyval("condition", data.condition);
  keyval("consequence", data.consequence);
  keyval("alternative", data.alternative);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::StatementWhile &data) {
  obj_beg();
  keyval("type", "statement");
  keyval("data", "while");
  keyval("condition", data.condition);
  keyval("child", data.child);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::StatementLoop &data) {
  obj_beg();
  keyval("type", "statement");
  keyval("data", "loop");
  keyval("child", data.child);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::StatementFor &data) {
  obj_beg();
  keyval("type", "statement");
  keyval("data", "for");
  keyval("initial", data.initial);
  keyval("condition", data.condition);
  keyval("increment", data.increment);
  keyval("child", data.child);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::StatementForeach &data) {
  obj_beg();
  keyval("type", "statement");
  keyval("data", "foreach");
  keyval("iterator", data.iterator);
  keyval("iteratorkind", data.iterator_kind);
  keyval("collection", data.collection);
  keyval("child", data.child);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::StatementMatch &) {
  obj_beg();
  keyval("type", "statement");
  keyval("data", "match");
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionIdentifier &data) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "identifier");
  keyval("name", data.name);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionVoid &) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "void");
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionContinuation &) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "continuation");
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionBool &data) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "boolean");
  keyval("value", data.value);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionNat &data) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "natural");
  keyval("value", data.value);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionInt &data) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "integer");
  keyval("value", data.value);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionReal &data) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "real");
  keyval("value", data.value);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionRealKeyword &data) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "realkeyword");
  keyval("kind", data.kind);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionChar &data) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "charac");
  keyval("value", data.value);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionString &data) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "string");
  keyval("value", data.value);
  keyval("rawvalue", data.raw_value);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionTuple &data) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "tuple");
  keyval("children", data.children);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionUnaryOp &data) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "unaryoperation");
  keyval("kind", data.kind);
  keyval("child", data.child);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionBinaryOp &data) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "binaryoperation");
  keyval("kind", data.kind);
  keyval("left", data.left);
  keyval("right", data.right);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionRange &data) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "range");
  keyval("left", data.left);
  keyval("right", data.right);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionVector &data) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "vector");
  keyval("children", data.children);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionArray &data) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "array");
  keyval("children", data.children);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionDictionary &data) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "dictionary");
  keyval("children", data.children);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionAssignment &data) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "assignment");
  keyval("kind", data.kind);
  keyval("assignee", data.assignee);
  keyval("child", data.child);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionCall &data) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "call");
  keyval("callee", data.callee);
  keyval("children", data.children);
  obj_end();
}

void JsonSerializer::handle(st::Node &, st::ExpressionLambda &data) {
  obj_beg();
  keyval("type", "expression");
  keyval("data", "lambda");
  keyval("parameters", data.parameters);
  keyval("child", data.child);
  obj_end();
}

auto JsonSerializer::execute(Package &&pkg) noexcept -> std::string {
  _output.clear();
  _output << std::fixed;

  auto modules = std::vector<std::reference_wrapper<Module>>{};
  modules.reserve(pkg.modules.size());

  std::transform(
    std::begin(pkg.modules),
    std::end(pkg.modules),
    std::back_inserter(modules),
    [](auto &entry) -> Module & { return entry.second; });

  obj_beg();
  keyval("type", "package");
  keyval("main", pkg.main);
  keyval("modules", modules);
  obj_end();

  return _output.str();
}

} // namespace silk
