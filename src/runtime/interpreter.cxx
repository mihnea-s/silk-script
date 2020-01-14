#include <silk/ast/ast.h>
#include <silk/ast/expr.h>
#include <silk/ast/stmt.h>
#include <silk/ast/token.h>
#include <silk/error.h>
#include <silk/runtime/abstract/env.h>
#include <silk/runtime/abstract/obj.h>
#include <silk/runtime/functions.h>
#include <silk/runtime/interpreter.h>
#include <silk/runtime/interupt.h>
#include <silk/runtime/objects.h>
#include <silk/runtime/stdlib.h>

auto Interpreter::SimpleEnvironment::push_scope() -> void {
  _scopes.emplace_front();
}
auto Interpreter::SimpleEnvironment::pop_scope() -> void {
  _scopes.pop_front();
}

bool Interpreter::SimpleEnvironment::exists(const std::string& name) {
  for (auto& scope : _scopes) {
    if (scope.find(name) != std::end(scope)) { return true; }
  }

  return false;
}

ObjectPtr Interpreter::SimpleEnvironment::get(const std::string& name) {
  for (auto& scope : _scopes) {
    if (scope.find(name) != std::end(scope)) { return scope.at(name); }
  }

  return obj::make<Vid>();
}

ObjectPtr Interpreter::SimpleEnvironment::assign(
  const std::string& name, ObjectPtr&& value) {

  for (auto& scope : _scopes) {
    if (scope.find(name) != std::end(scope)) {
      scope.insert_or_assign(name, std::move(value));
      return scope.at(name);
    }
  }

  _scopes.front().insert_or_assign(name, std::move(value));
  return _scopes.front().at(name);
}

inline auto Interpreter::is_empty(const Expr::Expr& ex) -> bool {
  try {
    std::get<std::unique_ptr<Expr::Empty>>(ex);
    return true;
  } catch (std::bad_variant_access) { return false; };
}

inline auto Interpreter::is_empty(const Stmt::Stmt& ex) -> bool {
  try {
    std::get<std::unique_ptr<Stmt::Empty>>(ex);
    return true;
  } catch (std::bad_variant_access) { return false; };
}

inline auto Interpreter::shortcircuits(TokenType t) -> bool {
  return t == TokenType::sym_ampamp || t == TokenType::sym_pipepipe;
}

auto Interpreter::evaluate(const Expr::Empty&) -> ObjectPtr {
  throw obj::make<Vid>();
}

auto Interpreter::evaluate(const Expr::Unary& expr) -> ObjectPtr {
  auto obj = evaluate_expr(expr.operand);

  switch (expr.operation.type()) {
    case TokenType::sym_bang: {
      return obj::make(!obj->truthy());
    }

    case TokenType::sym_tilde: {
      auto& bits = obj::cast_to<BitModifiable>(obj);
      return ~bits;
    }

    case TokenType::sym_minus: {
      auto& num = obj::cast_to<Numerical>(obj);
      return -num;
    };

    default: {
      throw RuntimeError {"unsupported unary operation"};
    }
  }
}

auto Interpreter::evaluate(const Expr::Binary& expr) -> ObjectPtr {
  auto left_obj = evaluate_expr(expr.left_operand);

  if (shortcircuits(expr.operation.type())) {

    if (expr.operation.type() == TokenType::sym_pipepipe) {
      if (left_obj->truthy()) { return obj::make(true); }
    } else {
      if (!left_obj->truthy()) { return obj::make(false); }
    }

    return obj::make(evaluate_expr(expr.right_operand)->truthy());
  }

  auto right_obj = evaluate_expr(expr.right_operand);

  switch (expr.operation.type()) {

    case TokenType::sym_plus: {
      auto& left  = obj::cast_to<Concatenable>(left_obj);
      auto& right = obj::cast_to<Concatenable>(right_obj);
      return left + right;
    }

    case TokenType::sym_minus: {
      auto& left  = obj::cast_to<Numerical>(left_obj);
      auto& right = obj::cast_to<Numerical>(right_obj);
      return left - right;
    }

    case TokenType::sym_star: {
      auto& left  = obj::cast_to<Concatenable>(left_obj);
      auto& right = obj::cast_to<Concatenable>(right_obj);
      return left * right;
    }

    case TokenType::sym_slash: {
      auto& left  = obj::cast_to<Numerical>(left_obj);
      auto& right = obj::cast_to<Numerical>(right_obj);
      return left / right;
    }

    case TokenType::sym_starstar: {
      auto& left  = obj::cast_to<Numerical>(left_obj);
      auto& right = obj::cast_to<Numerical>(right_obj);
      return left.starstar(right);
    }

    case TokenType::sym_slashslash: {
      auto& left  = obj::cast_to<Numerical>(left_obj);
      auto& right = obj::cast_to<Numerical>(right_obj);
      return left.slashslash(right);
    }

    case TokenType::sym_percent: {
      auto& left  = obj::cast_to<BitModifiable>(left_obj);
      auto& right = obj::cast_to<BitModifiable>(right_obj);
      return left % right;
    }

    case TokenType::sym_equalequal: {
      return *left_obj == right_obj;
    }

    case TokenType::sym_bangequal: {
      return obj::make(!(*left_obj == right_obj)->truthy());
    }

    case TokenType::sym_gt: {
      auto& left  = obj::cast_to<Orderable>(left_obj);
      auto& right = obj::cast_to<Orderable>(right_obj);
      return left > right;
    }

    case TokenType::sym_lt: {
      auto& left  = obj::cast_to<Orderable>(left_obj);
      auto& right = obj::cast_to<Orderable>(right_obj);
      return left < right;
    }

    case TokenType::sym_gtequal: {
      auto& left  = obj::cast_to<Orderable>(left_obj);
      auto& right = obj::cast_to<Orderable>(right_obj);
      return left >= right;
    }

    case TokenType::sym_ltequal: {
      auto& left  = obj::cast_to<Orderable>(left_obj);
      auto& right = obj::cast_to<Orderable>(right_obj);
      return left <= right;
    }

    case TokenType::sym_amp: {
      auto& left  = obj::cast_to<BitModifiable>(left_obj);
      auto& right = obj::cast_to<BitModifiable>(right_obj);
      return left & right;
    }

    case TokenType::sym_pipe: {
      auto& left  = obj::cast_to<BitModifiable>(left_obj);
      auto& right = obj::cast_to<BitModifiable>(right_obj);
      return left | right;
    }

    case TokenType::sym_caret: {
      auto& left  = obj::cast_to<BitModifiable>(left_obj);
      auto& right = obj::cast_to<BitModifiable>(right_obj);
      return left ^ right;
    }

    default: {
      throw RuntimeError {"unsupported binary operation"};
    }
  }
}

auto Interpreter::evaluate(const Expr::IntLiteral& integer) -> ObjectPtr {
  return obj::make(integer.value);
}

auto Interpreter::evaluate(const Expr::DoubleLiteral& dbl) -> ObjectPtr {
  return obj::make(dbl.value);
}

auto Interpreter::evaluate(const Expr::BoolLiteral& boolean) -> ObjectPtr {
  return obj::make(boolean.value);
}

auto Interpreter::evaluate(const Expr::StringLiteral& str) -> ObjectPtr {
  return obj::make(str.value);
}

auto Interpreter::evaluate(const Expr::VidLiteral&) -> ObjectPtr {
  return obj::make<Vid>();
}

auto Interpreter::evaluate(const Expr::Assignment& assignment)
  -> ObjectPtr {
  if (!_env.exists(assignment.name)) {
    throw RuntimeError {"assignment to undeclared variable"};
  }

  return _env.assign(assignment.name, evaluate_expr(assignment.expr));
}

auto Interpreter::evaluate(const Expr::Identifier& id) -> ObjectPtr {
  if (!_env.exists(id.value)) {
    throw RuntimeError {"undeclared variable used"};
  }

  return _env.get(id.value);
}

auto Interpreter::evaluate(const Expr::Grouping& group) -> ObjectPtr {
  return evaluate_expr(group.inner);
}

auto Interpreter::evaluate(const Expr::Call& call) -> ObjectPtr {
  auto  callee_obj = evaluate_expr(call.callee);
  auto& callee     = obj::cast_to<Callable>(callee_obj);

  if (call.args.size() != callee.arity()) {
    throw RuntimeError {
      "function called with the wrong number of arguments"};
  }

  _env.push_scope();

  for (auto i = 0; i < callee.arity(); i++) {
    _env.assign(callee.argument(i), evaluate_expr(call.args.at(i)));
  }

  auto ret = obj::make<Vid>();

  try {
    callee.call(_env);
  } catch (Interupt& interupt) {
    if (interupt.type == Interupt::Type::fct_return) {
      ret = std::move(interupt.ret);
    } else {
      throw RuntimeError {"invalid interupt out of function"};
    }
  }

  _env.pop_scope();
  return ret;
}

auto Interpreter::evaluate(const Expr::Lambda& lambda) -> ObjectPtr {
  auto& body = const_cast<Stmt::Stmt&>(lambda.body);
  return obj::make<Function>(body, lambda.parameters, *this);
}

auto Interpreter::execute(const Stmt::Empty&) -> std::nullptr_t {
  throw RuntimeError {"invalid interpreter state"};
  return nullptr;
}

auto Interpreter::execute(const Stmt::Entrypoint&) -> std::nullptr_t {
  return nullptr;
}

auto Interpreter::execute(const Stmt::Package& pkg) -> std::nullptr_t {
  return nullptr;
}

auto Interpreter::execute(const Stmt::Import& imprt) -> std::nullptr_t {
  StdLib::load_library(imprt.name, _env);
  return nullptr;
}

auto Interpreter::execute(const Stmt::Forwarding& fwd) -> std::nullptr_t {
  StdLib::load_library(fwd.name, _env);
  return nullptr;
}

auto Interpreter::execute(const Stmt::Export& exprt) -> std::nullptr_t {
  return nullptr;
}

auto Interpreter::execute(const Stmt::Variable& var) -> std::nullptr_t {
  auto init =
    is_empty(var.init) ? obj::make<Vid>() : evaluate_expr(var.init);
  _env.assign(var.name, std::move(init));
  return nullptr;
}

auto Interpreter::execute(const Stmt::Function& fct) -> std::nullptr_t {
  auto& body = const_cast<Stmt::Stmt&>(fct.body);
  auto  obj  = obj::make<Function>(body, fct.parameters, *this);
  _env.assign(fct.name, std::move(obj));
  return nullptr;
}

auto Interpreter::execute(const Stmt::Struct&) -> std::nullptr_t {
  return nullptr;
}

auto Interpreter::execute(const Stmt::Constructor&) -> std::nullptr_t {
  return nullptr;
}

auto Interpreter::execute(const Stmt::Destructor&) -> std::nullptr_t {
  return nullptr;
}

auto Interpreter::execute(const Stmt::Loop& l) -> std::nullptr_t {
  while (evaluate_expr(l.clause)->truthy()) {
    try {
      execute_stmt(l.body);
    } catch (Interupt& interupt) {
      if (interupt.type == Interupt::Type::loop_break) { break; }
    }
  }

  return nullptr;
}

auto Interpreter::execute(const Stmt::LoopInterupt& i) -> std::nullptr_t {
  throw Interupt {
    i.should_continue ? Interupt::Type::loop_continue
                      : Interupt::Type::loop_break,
  };
}

auto Interpreter::execute(const Stmt::Conditional& cond) -> std::nullptr_t {
  if (evaluate_expr(cond.clause)->truthy()) {
    execute_stmt(cond.true_stmt);
  } else if (!is_empty(cond.false_stmt)) {
    execute_stmt(cond.false_stmt);
  }

  return nullptr;
}

auto Interpreter::execute(const Stmt::Block& blk) -> std::nullptr_t {
  _env.push_scope();

  for (auto&& stmt : blk.body) {
    try {
      execute_stmt(stmt);

    } catch (Interupt& interupt) {
      if (interupt.type != Interupt::Type::loop_continue) {
        _env.pop_scope();
      }
      throw interupt;
    }
  }

  _env.pop_scope();
  return nullptr;
}

auto Interpreter::execute(const Stmt::Return& r) -> std::nullptr_t {
  auto ret = obj::make<Vid>();

  if (!is_empty(r.ret)) { ret = evaluate_expr(r.ret); }

  throw Interupt {
    Interupt::Type::fct_return,
    std::move(ret),
  };
}

auto Interpreter::execute(const Stmt::ExprStmt& e) -> std::nullptr_t {
  evaluate_expr(e.expr);
  return nullptr;
}

auto Interpreter::interpret(AST& ast) -> void {
  try {
    ast.evaluate_with(*this);
  } catch (RuntimeError e) {
    _error_callback(e);
    return;
  }
}