#pragma once

#include <functional>
#include <list>
#include <map>

#include "../ast/ast.h"
#include "../ast/expr.h"
#include "../ast/stmt.h"
#include "../error.h"

#include "abstract/env.h"
#include "abstract/obj.h"

struct Interpreter :
    Expr::Visitor<ObjectPtr>,
    Stmt::Visitor<std::nullptr_t> {
  private:
  struct SimpleEnvironment : Environment<ObjectPtr> {
    std::list<std::map<std::string, ObjectPtr>> _scopes {};

    SimpleEnvironment() {
      _scopes.emplace_front();
    }

    auto push_scope() -> void override;
    auto pop_scope() -> void override;

    bool      exists(const std::string& name) override;
    ObjectPtr get(const std::string& name) override;
    ObjectPtr assign(const std::string& name, ObjectPtr&& value) override;
  };

  SimpleEnvironment _env {};

  inline auto is_empty(const Expr::Expr& ex) -> bool;
  inline auto is_empty(const Stmt::Stmt& ex) -> bool;

  inline auto interpolate_str(std::string str) -> std::string;

  inline auto shortcircuits(TokenType t) -> bool;

  auto evaluate(const Expr::Empty&) -> ObjectPtr override;

  auto evaluate(const Expr::Unary& expr) -> ObjectPtr override;
  auto evaluate(const Expr::Binary& expr) -> ObjectPtr override;

  auto evaluate(const Expr::IntLiteral& integer) -> ObjectPtr override;
  auto evaluate(const Expr::DoubleLiteral& dbl) -> ObjectPtr override;
  auto evaluate(const Expr::BoolLiteral& boolean) -> ObjectPtr override;
  auto evaluate(const Expr::StringLiteral& str) -> ObjectPtr override;
  auto evaluate(const Expr::VidLiteral&) -> ObjectPtr override;

  auto evaluate(const Expr::Assignment& assignment) -> ObjectPtr override;
  auto evaluate(const Expr::Identifier& id) -> ObjectPtr override;
  auto evaluate(const Expr::Grouping& group) -> ObjectPtr override;

  auto evaluate(const Expr::Call& call) -> ObjectPtr override;
  auto evaluate(const Expr::Lambda& group) -> ObjectPtr override;

  auto execute(const Stmt::Empty&) -> std::nullptr_t override;

  auto execute(const Stmt::Entrypoint&) -> std::nullptr_t override;
  auto execute(const Stmt::Package& pkg) -> std::nullptr_t override;
  auto execute(const Stmt::Import& imprt) -> std::nullptr_t override;
  auto execute(const Stmt::Forwarding& fwd) -> std::nullptr_t override;
  auto execute(const Stmt::Export& exprt) -> std::nullptr_t override;

  auto execute(const Stmt::Variable& var) -> std::nullptr_t override;
  auto execute(const Stmt::Function& fct) -> std::nullptr_t override;

  auto execute(const Stmt::Struct&) -> std::nullptr_t override;
  auto execute(const Stmt::Constructor&) -> std::nullptr_t override;
  auto execute(const Stmt::Destructor&) -> std::nullptr_t override;

  auto execute(const Stmt::Loop& l) -> std::nullptr_t override;
  auto execute(const Stmt::LoopInterupt& i) -> std::nullptr_t override;
  auto execute(const Stmt::Conditional& cond) -> std::nullptr_t override;
  auto execute(const Stmt::Block& blk) -> std::nullptr_t override;
  auto execute(const Stmt::Return& r) -> std::nullptr_t override;
  auto execute(const Stmt::ExprStmt& e) -> std::nullptr_t override;

  public:
  auto interpret(AST& ast) -> void;

  std::function<void(RuntimeError&)> _error_callback;
  void set_error_callback(std::function<void(RuntimeError&)> cb) {
    _error_callback = cb;
  }
};
