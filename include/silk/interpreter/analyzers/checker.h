#pragma once

#include <map>
#include <string>
#include <vector>

#include "../../common/error.h"
#include "../ast/ast.h"
#include "../ast/expr.h"
#include "../ast/stmt.h"

struct Checker : Expr::Visitor<std::string>, Stmt::Visitor<std::nullptr_t> {
  private:
  std::vector<ParsingError> _errors;

  std::vector<std::map<std::string, std::string>> _scopes;

  auto add_error(const std::string&) -> void;

  template <class... Args>
  auto not_dyn(Args... args) -> bool {
    for (auto& type : std::vector {args...}) {
      if (type.empty()) return true;
    }
    return false;
  }

  template <class T, class... Args>
  auto not_in(T t, Args... args) -> bool {
    for (auto& type : std::vector {args...}) {
      if (t == type) return false;
    }
    return true;
  }

  auto push_scope() -> void;
  auto pop_scope() -> void;

  auto var_type(const std::string&) -> std::string;

  auto evaluate(const Expr::Unary& expr) -> std::string override;
  auto evaluate(const Expr::Binary& expr) -> std::string override;

  auto evaluate(const Expr::IntLiteral& integer) -> std::string override;
  auto evaluate(const Expr::DoubleLiteral& dbl) -> std::string override;
  auto evaluate(const Expr::BoolLiteral& boolean) -> std::string override;
  auto evaluate(const Expr::StringLiteral& str) -> std::string override;
  auto evaluate(const Expr::Vid&) -> std::string override;
  auto evaluate(const Expr::Lambda& group) -> std::string override;

  auto evaluate(const Expr::Assignment& assignment) -> std::string override;
  auto evaluate(const Expr::Identifier& id) -> std::string override;
  auto evaluate(const Expr::Grouping& group) -> std::string override;

  auto evaluate(const Expr::Call& call) -> std::string override;
  auto evaluate(const Expr::Get& get) -> std::string override;

  auto execute(const Stmt::Empty&) -> std::nullptr_t override;

  auto execute(const Stmt::Main&) -> std::nullptr_t override;
  auto execute(const Stmt::Package& pkg) -> std::nullptr_t override;
  auto execute(const Stmt::Import& imprt) -> std::nullptr_t override;

  auto execute(const Stmt::Variable& var) -> std::nullptr_t override;
  auto execute(const Stmt::Function& fct) -> std::nullptr_t override;
  auto execute(const Stmt::Struct&) -> std::nullptr_t override;

  auto execute(const Stmt::Loop& l) -> std::nullptr_t override;
  auto execute(const Stmt::Interrupt& i) -> std::nullptr_t override;
  auto execute(const Stmt::Conditional& cond) -> std::nullptr_t override;
  auto execute(const Stmt::Block& blk) -> std::nullptr_t override;
  auto execute(const Stmt::ExprStmt& e) -> std::nullptr_t override;

  public:
  auto has_error() const -> bool;
  auto clear_errors() -> void;
  auto errors() -> std::vector<ParsingError>&;

  auto check(AST& ast) -> void;
};
