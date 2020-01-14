#include <algorithm>
#include <cstddef>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>

#include <fmt/core.h>
#include <fmt/format.h>

#include <silk/ast/ast.h>
#include <silk/ast/expr.h>
#include <silk/ast/stmt.h>

#include "tok2str.h"

#define Black   "\u001b[30m"
#define Red     "\u001b[31m"
#define Green   "\u001b[32m"
#define Yellow  "\u001b[33m"
#define Blue    "\u001b[34m"
#define Magenta "\u001b[35m"
#define Cyan    "\u001b[36m"
#define White   "\u001b[37m"
#define Reset   "\u001b[0m"

namespace _internal {

struct AstPrinter :
    Expr::Visitor<std::string>,
    Stmt::Visitor<std::nullptr_t> {
  public:
  inline auto is_empty(const Expr::Expr& ex) -> bool {
    try {
      std::get<std::unique_ptr<Expr::Empty>>(ex);
      return true;
    } catch (std::bad_variant_access) { return false; };
  }

  inline auto is_empty(const Stmt::Stmt& ex) -> bool {
    try {
      std::get<std::unique_ptr<Stmt::Empty>>(ex);
      return true;
    } catch (std::bad_variant_access) { return false; };
  }

  std::nullptr_t execute(const Stmt::Empty&) override {
    return nullptr;
  }

  std::nullptr_t execute(const Stmt::Variable& var) override {
    fmt::print("let {}", var.name);

    if (is_empty(var.init)) {
      fmt::print(" = {};\n", evaluate_expr(var.init));
    } else {
      fmt::print(": {};\n", var.type);
    }
    return nullptr;
  }

  std::nullptr_t execute(const Stmt::Function& fun) override {
    fmt::print("fct {} ", fun.name);

    if (fun.parameters.empty()) {
      fmt::print("{{\n");
    } else {
      fmt::print("(");

      for (auto param : fun.parameters) {
        fmt::print("{}, ", param);
      }

      fmt::print(") {{\n");
    }

    execute_stmt(fun.body);

    fmt::print("}};\n");
    return nullptr;
  }

  std::nullptr_t execute(const Stmt::Struct& s) override {
    fmt::print(
      "struct {} / {} {{\n",
      s.name,
      s.parent.empty() ? "Object" : s.parent);

    for (auto&& stmt : s.body) {
      fmt::print("  ");
      execute_stmt(stmt);
    }

    fmt::print("}};\n");

    return nullptr;
  }

  std::nullptr_t execute(const Stmt::Constructor& ctor) override {
    fmt::print("ctor ");

    if (ctor.parameters.empty()) {
      fmt::print("{{\n");
    } else {
      fmt::print("(");

      for (auto param : ctor.parameters) {
        fmt::print("{}, ", param);
      }

      fmt::print(") {{\n");
    }

    execute_stmt(ctor.body);
    fmt::print("}};\n");
    return nullptr;
  }

  std::nullptr_t execute(const Stmt::Destructor& dtor) override {
    fmt::print("dtor {{\n");
    execute_stmt(dtor.body);
    fmt::print("}};\n");
    return nullptr;
  }

  std::nullptr_t execute(const Stmt::Entrypoint& s) override {
    fmt::print("entrypoint;\n");
    return nullptr;
  }

  std::nullptr_t execute(const Stmt::Package& p) override {
    fmt::print("pkg {};\n", p.name);
    return nullptr;
  }

  std::nullptr_t execute(const Stmt::Import& p) override {
    fmt::print("use {};\n", p.name);
    return nullptr;
  }

  std::nullptr_t execute(const Stmt::Forwarding& p) override {
    fmt::print("with {};\n", p.name);
    return nullptr;
  }

  std::nullptr_t execute(const Stmt::Export& p) override {
    fmt::print("exported:\n");
    execute_stmt(p.decl);
    return nullptr;
  }

  std::nullptr_t execute(const Stmt::Conditional& p) override {

    fmt::print("if ({}) then {{\n", evaluate_expr(p.clause));
    execute_stmt(p.true_stmt);

    if (!is_empty(p.false_stmt)) {
      fmt::print("}} else {{\n");
      execute_stmt(p.false_stmt);
    }

    fmt::print("}};\n");
    return nullptr;
  }

  std::nullptr_t execute(const Stmt::Loop& p) override {

    fmt::print("while ({}) do {{\n", evaluate_expr(p.clause));
    execute_stmt(p.body);
    fmt::print("}};\n");

    return nullptr;
  }

  std::nullptr_t execute(const Stmt::LoopInterupt& p) override {
    fmt::print("{};", p.should_continue ? "continue" : "break");
    return nullptr;
  }

  std::nullptr_t execute(const Stmt::Block& blk) override {

    for (auto&& stmt : blk.body) {
      fmt::print("  ");
      execute_stmt(stmt);
    }

    return nullptr;
  }

  std::nullptr_t execute(const Stmt::ExprStmt& p) override {
    fmt::print("{};\n", evaluate_expr(p.expr));
    return nullptr;
  }

  std::nullptr_t execute(const Stmt::Return& s) override {
    fmt::print("return {};\n", evaluate_expr(s.ret));
    return nullptr;
  }

  std::string evaluate(const Expr::Empty&) override {
    return std::string {};
  }

  std::string evaluate(const Expr::Unary& u) override {
    return fmt::format(
      "({} {})", tok2str(u.operation), evaluate_expr(u.operand));
  }

  std::string evaluate(const Expr::Binary& u) override {
    return fmt::format(
      "({} {} {})",
      evaluate_expr(u.left_operand),
      tok2str(u.operation),
      evaluate_expr(u.right_operand));
  }

  std::string evaluate(const Expr::Grouping& u) override {
    return fmt::format("[{}]", evaluate_expr(u.inner));
  }

  std::string evaluate(const Expr::IntLiteral& u) override {
    return fmt::format("int {}", u.value);
  }

  std::string evaluate(const Expr::DoubleLiteral& u) override {
    return fmt::format("dbl {}", u.value);
  }

  std::string evaluate(const Expr::BoolLiteral& u) override {
    return fmt::format("{}", u.value ? "true" : "false");
  }

  std::string evaluate(const Expr::StringLiteral& u) override {
    return fmt::format("str {}", u.value);
  }

  std::string evaluate(const Expr::VidLiteral& u) override {
    return fmt::format(Red "vid" Reset);
  }

  std::string evaluate(const Expr::Identifier& u) override {
    return fmt::format("@{}", u.value);
  }

  std::string evaluate(const Expr::Assignment& u) override {
    return fmt::format("@{} <- {}", u.name, evaluate_expr(u.expr));
  }

  std::string evaluate(const Expr::Call& c) override {
    std::ostringstream ss {};

    for (auto&& arg : c.args) {
      if (!ss.str().empty()) { ss << ", "; }
      ss << evaluate_expr(arg);
    }

    return fmt::format("{}({})", evaluate_expr(c.callee), ss.str());
  }

  std::string evaluate(const Expr::Lambda& group) override {
    return "[lambda]";
  }
};

} // namespace _internal

void ast_print(AST& ast) {
  auto prnter = _internal::AstPrinter {};
  ast.evaluate_with(prnter);
}