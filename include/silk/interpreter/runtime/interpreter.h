#pragma once

#include <functional>
#include <istream>
#include <list>
#include <map>
#include <ostream>
#include <stack>

#include "../../common/error.h"
#include "../ast/ast.h"
#include "../ast/expr.h"
#include "../ast/stmt.h"
#include "../interfaces/obj.h"

struct Interpreter : Expr::Visitor<ObjectPtr>, Stmt::Visitor<std::nullptr_t> {
  struct Environment {
private:
    struct Scope {
      ObjectPtr              value;
      std::string            name;
      std::shared_ptr<Scope> parent;

      Scope(ObjectPtr v, std::string n, std::shared_ptr<Scope> p) :
          value(std::move(v)), name(n), parent(p) {
      }
    };

    struct ScopeWrapper {
      Environment& _parent;

      ScopeWrapper(Environment& parent) : _parent(parent) {
      }

      ~ScopeWrapper() {
        _parent.pop();
      }
    };

    std::shared_ptr<Scope>             _current;
    std::stack<std::shared_ptr<Scope>> _scopes;

    auto pop() -> void;

public:
    using SavedScope = std::shared_ptr<Scope>;

    auto new_scope() -> ScopeWrapper;
    auto from_scope(std::shared_ptr<Scope>) -> ScopeWrapper;

    auto save_scope() -> std::shared_ptr<Scope>;

    auto define(const std::string& name, ObjectPtr& value) -> void;

    auto get(const std::string& name) -> ObjectPtr;
    auto assign(const std::string& name, ObjectPtr& value) -> ObjectPtr;
  };

  struct Interrupt {
    ObjectPtr             value;
    Stmt::Interrupt::Type type;
  };

  private:
  Environment _env {};

  std::vector<RuntimeError> _errors;

  inline auto interpolate_str(std::string str) -> std::string;

  inline auto shortcircuits(TokenType t) -> bool;

  auto evaluate(const Expr::Unary& expr) -> ObjectPtr override;
  auto evaluate(const Expr::Binary& expr) -> ObjectPtr override;

  auto evaluate(const Expr::IntLiteral& integer) -> ObjectPtr override;
  auto evaluate(const Expr::DoubleLiteral& dbl) -> ObjectPtr override;
  auto evaluate(const Expr::BoolLiteral& boolean) -> ObjectPtr override;
  auto evaluate(const Expr::StringLiteral& str) -> ObjectPtr override;
  auto evaluate(const Expr::Vid&) -> ObjectPtr override;
  auto evaluate(const Expr::Lambda& group) -> ObjectPtr override;

  auto evaluate(const Expr::Assignment& assignment) -> ObjectPtr override;
  auto evaluate(const Expr::Identifier& id) -> ObjectPtr override;
  auto evaluate(const Expr::Grouping& group) -> ObjectPtr override;

  auto evaluate(const Expr::Call& call) -> ObjectPtr override;
  auto evaluate(const Expr::Get& get) -> ObjectPtr override;

  auto execute(const Stmt::Empty&) -> std::nullptr_t override;

  auto execute(const Stmt::Main&) -> std::nullptr_t override;
  auto execute(const Stmt::Package& pkg) -> std::nullptr_t override;
  auto execute(const Stmt::Import& imprt) -> std::nullptr_t override;

  auto execute(const Stmt::Variable& var) -> std::nullptr_t override;
  auto execute(const Stmt::Function& fct) -> std::nullptr_t override;
  auto execute(const Stmt::Struct&) -> std::nullptr_t override;

  auto execute(const Stmt::Loop& l) -> std::nullptr_t override;
  auto execute(const Stmt::Conditional& cond) -> std::nullptr_t override;
  auto execute(const Stmt::Block& blk) -> std::nullptr_t override;
  auto execute(const Stmt::Interrupt& r) -> std::nullptr_t override;
  auto execute(const Stmt::ExprStmt& e) -> std::nullptr_t override;

  public:
  auto has_error() const -> bool;
  auto clear_errors() -> void;
  auto errors() -> std::vector<RuntimeError>&;

  auto interpret(AST& ast, std::istream&, std::ostream&) -> void;
};
