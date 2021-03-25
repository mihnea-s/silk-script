#include <silk/tools/stprinter.h>

#include <silk/syntax/tree.h>

namespace silk {

void TreePrinter::indent() {
  _indentation_level++;
}

void TreePrinter::unindent() {
  _indentation_level--;
}

void TreePrinter::append_semi() {
  _output << ';' << std::endl;
}

void TreePrinter::append_line() {
  _output << std::endl;
}

void TreePrinter::handle(st::Node &node, st::Comment &data) {
  if (data.placement == st::Comment::BEFORE) {
    append_line("// {}", data.text);
  }

  handle_node(*data.child);

  if (data.placement == st::Comment::AFTER) { //
    append_line("// {}", data.text);
  }
}

void TreePrinter::handle(st::Node &node, st::ModuleMain &data) {
  append_line("main;");
}

void TreePrinter::handle(st::Node &node, st::ModuleDeclaration &data) {
  append_line("pkg '{}';", data.path);
}

void TreePrinter::handle(st::Node &node, st::ModuleImport &data) {
  append("use '{}'", data.name);

  if (!data.imports.empty()) {
    append(" / ");

    for (auto &&import : data.imports) {
      append("{}, ", import);
    }
  }

  append_semi();
}

void TreePrinter::handle(st::Node &node, st::DeclarationFunction &data) {
  append("fun {}", data.name);
  handle_node(*data.child);
}

void TreePrinter::handle(st::Node &node, st::DeclarationEnum &data) {
  append_line("enum {};", data.name);
}

void TreePrinter::handle(st::Node &node, st::DeclarationObject &data) {
  append_line("dll '{}';", data.name);
}

void TreePrinter::handle(st::Node &node, st::DeclarationDynamicLibrary &data) {
  append_line("obj {};", data.name);
}

void TreePrinter::handle(st::Node &node, st::DeclarationMacro &data) {
  append_line("macro {};", data.name);
}

void TreePrinter::handle(st::Node &node, st::StatementEmpty &data) {
  append_semi();
}

void TreePrinter::handle(st::Node &node, st::StatementExpression &data) {
  handle_node(*data.child);
  append_semi();
}

void TreePrinter::handle(st::Node &node, st::StatementBlock &data) {
  append_line("{");
  indent();

  for (auto &&stmt : data.children) {
    handle_node(stmt);
  }

  unindent();
  append_line("}");
}

void TreePrinter::handle(st::Node &node, st::StatementCircuit &data) {
  append_line("${");
  indent();

  for (auto &&swtch : data.children) {
    append_line("{}:", swtch.first);

    indent();
    handle_node(swtch.second);
    unindent();
  }

  unindent();
  append_line("}");
}

void TreePrinter::handle(st::Node &node, st::StatementVariable &data) {
  append(
    "{} {} = ",
    data.kind == st::StatementVariable::DEF ? "def" : "let",
    data.name);
  handle_node(*data.child);
  append_semi();
}

void TreePrinter::handle(st::Node &node, st::StatementConstant &data) {
  append("const {} = ", data.name);
  handle_node(*data.child);
  append_semi();
}

void TreePrinter::handle(st::Node &node, st::StatementReturn &data) {
  append("return ");

  if (data.continuation) {
    append("[");
    handle_node(*data.continuation);
    append("] ");
  }

  if (data.child) { handle_node(*data.child); }

  append_semi();
}

void TreePrinter::handle(st::Node &node, st::StatementSwitch &data) {
  append_line("switch {};", data.label);
}

void TreePrinter::handle(st::Node &node, st::StatementIterationControl &data) {
  append_line(
    "{};",
    data.kind == st::StatementIterationControl::BREAK ? "break" : "continue");
}

void TreePrinter::handle(st::Node &node, st::StatementIf &data) {
  append("if (");
  handle_node(*data.condition);
  append(") ");

  handle_node(*data.consequence);

  if (data.alternative) {
    append(" else ");
    handle_node(*data.alternative);
  }
}

void TreePrinter::handle(st::Node &node, st::StatementWhile &data) {
}

void TreePrinter::handle(st::Node &node, st::StatementLoop &data) {
}

void TreePrinter::handle(st::Node &node, st::StatementFor &data) {
}

void TreePrinter::handle(st::Node &node, st::StatementForeach &data) {
}

void TreePrinter::handle(st::Node &node, st::StatementMatch &data) {
}

void TreePrinter::handle(st::Node &node, st::ExpressionIdentifier &data) {
  append("{}", data.name);
}

void TreePrinter::handle(st::Node &node, st::ExpressionVoid &data) {
  append("void");
}

void TreePrinter::handle(st::Node &node, st::ExpressionContinuation &data) {
  append("return");
}

void TreePrinter::handle(st::Node &node, st::ExpressionBool &data) {
  append("{}", data.value);
}

void TreePrinter::handle(st::Node &node, st::ExpressionNat &data) {
  append("{}", data.value);
}

void TreePrinter::handle(st::Node &node, st::ExpressionInt &data) {
  append("{}", data.value);
}

void TreePrinter::handle(st::Node &node, st::ExpressionReal &data) {
  append("{}", data.value);
}

void TreePrinter::handle(st::Node &node, st::ExpressionRealKeyword &data) {
  switch (data.kind) {
    case st::ExpressionRealKeyword::PI: append("pi"); break;
    case st::ExpressionRealKeyword::TAU: append("tau"); break;
    case st::ExpressionRealKeyword::EULER: append("e"); break;
  }
}

void TreePrinter::handle(st::Node &node, st::ExpressionChar &data) {
  append("\"{}", (char)data.value);
}

void TreePrinter::handle(st::Node &node, st::ExpressionString &data) {
  append("'{}'", data.value);
}

void TreePrinter::handle(st::Node &node, st::ExpressionTuple &data) {
  append("(");

  for (auto &&elem : data.children) {
    handle_node(elem);
    append(",");
  }

  append(")");
}

void TreePrinter::handle(st::Node &node, st::ExpressionUnaryOp &data) {
  switch (data.kind) {
    case st::ExpressionUnaryOp::NEG: append("- "); break;
    case st::ExpressionUnaryOp::NOT: append("not "); break;
  }

  handle_node(*data.child);
}

void TreePrinter::handle(st::Node &node, st::ExpressionBinaryOp &data) {
  handle_node(*data.left);

  switch (data.kind) {
    case st::ExpressionBinaryOp::OR: append("or"); break;
    case st::ExpressionBinaryOp::AND: append("and"); break;
    case st::ExpressionBinaryOp::EQUAL: append("=="); break;
    case st::ExpressionBinaryOp::NOTEQ: append("!="); break;
    case st::ExpressionBinaryOp::LESS: append("<"); break;
    case st::ExpressionBinaryOp::LESSEQ: append("<="); break;
    case st::ExpressionBinaryOp::GREATER: append(">"); break;
    case st::ExpressionBinaryOp::GREATEREQ: append(">="); break;
    case st::ExpressionBinaryOp::ADD: append("+"); break;
    case st::ExpressionBinaryOp::SUB: append("-"); break;
    case st::ExpressionBinaryOp::DIV: append("/"); break;
    case st::ExpressionBinaryOp::RDIV: append("//"); break;
    case st::ExpressionBinaryOp::MUL: append("*"); break;
    case st::ExpressionBinaryOp::POW: append("**"); break;
    case st::ExpressionBinaryOp::MOD: append("%"); break;
    case st::ExpressionBinaryOp::INDEX: append("."); break;
    case st::ExpressionBinaryOp::APPEND: append("~"); break;
    case st::ExpressionBinaryOp::MERGE: append("|"); break;
  }

  handle_node(*data.right);
}

void TreePrinter::handle(st::Node &node, st::ExpressionRange &data) {
  handle_node(*data.left);
  append("..");
  handle_node(*data.right);
}

void TreePrinter::handle(st::Node &node, st::ExpressionVector &data) {
}
void TreePrinter::handle(st::Node &node, st::ExpressionArray &data) {
}
void TreePrinter::handle(st::Node &node, st::ExpressionDictionary &data) {
}
void TreePrinter::handle(st::Node &node, st::ExpressionAssignment &data) {
}
void TreePrinter::handle(st::Node &node, st::ExpressionCall &data) {
}
void TreePrinter::handle(st::Node &node, st::ExpressionLambda &data) {
}

auto TreePrinter::execute(Module &&mod) noexcept -> std::string {
  _output.clear();
  _indentation_level = 0;

  for (auto &&node : mod.tree) {
    handle_node(*node);
    append_line();
  }

  return _output.str();
}

} // namespace silk
