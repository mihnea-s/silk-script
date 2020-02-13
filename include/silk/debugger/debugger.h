#pragma once

#include <functional>
#include <istream>
#include <list>
#include <map>
#include <ostream>
#include <stack>

#include <vm/value.h>

#include "../common/ast.h"
#include "../common/error.h"

using ValuePtr = std::shared_ptr<Value>;

struct Debugger : ASTVisitor<ValuePtr>, public ErrorReporter {
  class Environment {
private:
    struct Scope {
      ValuePtr               value;
      std::string            name;
      std::shared_ptr<Scope> parent;

      Scope(ValuePtr v, std::string n, std::shared_ptr<Scope> p) :
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

    auto define(const std::string& name, ValuePtr& value) -> void;

    auto get(const std::string& name) -> ValuePtr;
    auto assign(const std::string& name, ValuePtr& value) -> ValuePtr;
  };

  struct Interruption {
    ValuePtr                 value;
    Interrupt::InterruptType type;
  };

  private:
  Environment _env {};

  inline auto string_representation(ValuePtr value) -> std::string;
  inline auto interpolate_str(std::string str) -> std::string;
  inline auto shortcircuits(TokenType t) -> bool;

  auto evaluate(const Unary& expr) -> ValuePtr final;
  auto evaluate(const Binary& expr) -> ValuePtr final;
  auto evaluate(const IntLiteral& integer) -> ValuePtr final;
  auto evaluate(const RealLiteral& dbl) -> ValuePtr final;
  auto evaluate(const BoolLiteral& boolean) -> ValuePtr final;
  auto evaluate(const StringLiteral& str) -> ValuePtr final;
  auto evaluate(const Vid&) -> ValuePtr final;
  auto evaluate(const Constant&) -> ValuePtr final;
  auto evaluate(const Lambda& group) -> ValuePtr final;
  auto evaluate(const Assignment& assignment) -> ValuePtr final;
  auto evaluate(const Identifier& id) -> ValuePtr final;
  auto evaluate(const Grouping& group) -> ValuePtr final;
  auto evaluate(const Call& call) -> ValuePtr final;
  auto evaluate(const Get& get) -> ValuePtr final;

  auto execute(const Empty&) -> ValuePtr final;
  auto execute(const Package& pkg) -> ValuePtr final;
  auto execute(const Variable& var) -> ValuePtr final;
  auto execute(const Function& fct) -> ValuePtr final;
  auto execute(const Struct&) -> ValuePtr final;
  auto execute(const Loop& l) -> ValuePtr final;
  auto execute(const Conditional& cond) -> ValuePtr final;
  auto execute(const Block& blk) -> ValuePtr final;
  auto execute(const Interrupt& r) -> ValuePtr final;
  auto execute(const ExprStmt& e) -> ValuePtr final;

  public:
  auto debug(AST& ast, std::istream&, std::ostream&) -> void;
};
