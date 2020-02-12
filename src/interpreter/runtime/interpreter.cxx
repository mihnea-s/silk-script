#include <cassert>
#include <istream>
#include <memory>
#include <ostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include <silk/common/error.h>
#include <silk/common/token.h>
#include <silk/interpreter/ast/ast.h>
#include <silk/interpreter/ast/expr.h>
#include <silk/interpreter/ast/stmt.h>
#include <silk/interpreter/interfaces/obj.h>
#include <silk/interpreter/runtime/functions.h>
#include <silk/interpreter/runtime/interpreter.h>
#include <silk/interpreter/runtime/primitives.h>
#include <silk/interpreter/runtime/stdlib.h>
#include <silk/interpreter/runtime/structs.h>

auto Interpreter::Environment::pop() -> void {
  _current = _scopes.top();
  _scopes.pop();
}

auto Interpreter::Environment::new_scope() -> ScopeWrapper {
  _scopes.push(_current);
  return ScopeWrapper(*this);
}

auto Interpreter::Environment::from_scope(std::shared_ptr<Scope> old)
  -> ScopeWrapper {
  _scopes.push(_current);
  _current = old;
  return ScopeWrapper(*this);
}

auto Interpreter::Environment::save_scope() -> std::shared_ptr<Scope> {
  return _current;
}

auto Interpreter::Environment::define(const std::string& name, ObjectPtr& value)
  -> void {
  auto new_scope = std::make_shared<Scope>(value, name, _current);
  _current       = new_scope;
}

ObjectPtr Interpreter::Environment::get(const std::string& name) {
  auto searched = _current;

  while (searched && searched->name != name) {
    searched = searched->parent;
  }

  if (!searched) { throw RuntimeError {"use of undefined variable"}; }

  return searched->value;
}

ObjectPtr
Interpreter::Environment::assign(const std::string& name, ObjectPtr& value) {
  auto searched = _current;

  while (searched->name != name) {
    searched = searched->parent;
  }

  if (!searched) {
    throw RuntimeError {
      "assignment of undefined variable",
    };
  }

  searched->value = value;
  return searched->value;
}

inline auto Interpreter::interpolate_str(std::string str) -> std::string {
  std::ostringstream output {};

  std::regex  var_regex(R"#(\$(\w+)\$)#");
  std::smatch match;

  while (std::regex_search(str, match, var_regex)) {
    assert(match.size() == 2);

    output << match.prefix();
    output << _env.get(match[1])->string();
    str = match.suffix();
  }

  output << str;

  return output.str();
}

inline auto Interpreter::shortcircuits(TokenType t) -> bool {
  return t == TokenType::sym_ampamp || t == TokenType::sym_pipepipe;
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
  return obj::make(interpolate_str(str.value));
}

auto Interpreter::evaluate(const Expr::Vid&) -> ObjectPtr {
  return obj::make<Vid>();
}

auto Interpreter::evaluate(const Expr::Assignment& assignment) -> ObjectPtr {
  auto val = evaluate_expr(assignment.expr);
  return _env.assign(assignment.name, val);
}

auto Interpreter::evaluate(const Expr::Identifier& id) -> ObjectPtr {
  return _env.get(id.value);
}

auto Interpreter::evaluate(const Expr::Grouping& group) -> ObjectPtr {
  return evaluate_expr(group.inner);
}

auto Interpreter::evaluate(const Expr::Call& call) -> ObjectPtr {
  auto  callee_obj = evaluate_expr(call.callee);
  auto& callee     = obj::cast_to<Callable>(callee_obj);

  if (callee.arity() != -1 && call.args.size() != callee.arity()) {
    throw RuntimeError {"function called with the wrong number of arguments"};
  }

  auto args = std::vector<ObjectPtr> {};

  for (auto& arg : call.args) {
    args.push_back(evaluate_expr(arg));
  }

  auto ret = obj::make<Vid>();

  try {
    callee.call(args);
  } catch (Interrupt& interupt) {
    if (interupt.type == Stmt::Interrupt::Type::ret) {
      ret = std::move(interupt.value);
    } else {
      throw RuntimeError {"invalid interupt out of function"};
    }
  }

  return ret;
}

auto Interpreter::evaluate(const Expr::Get& get) -> ObjectPtr {
  auto  instance_obj = evaluate_expr(get.from);
  auto& instance     = obj::cast_to<Gettable>(instance_obj);
  return instance.get(get.property);
}

auto Interpreter::evaluate(const Expr::Lambda& lambda) -> ObjectPtr {
  auto& body = const_cast<Stmt::Stmt&>(lambda.body);
  return obj::make<Function>(
    body, lambda.parameters, *this, _env, _env.save_scope());
}

auto Interpreter::execute(const Stmt::Empty&) -> std::nullptr_t {
  return nullptr;
}

auto Interpreter::execute(const Stmt::Main&) -> std::nullptr_t {
  return nullptr;
}

auto Interpreter::execute(const Stmt::Package& pkg) -> std::nullptr_t {
  return nullptr;
}

auto Interpreter::execute(const Stmt::Import& imprt) -> std::nullptr_t {
  StdLib::load_library(imprt.name, _env);
  return nullptr;
}

auto Interpreter::execute(const Stmt::Variable& var) -> std::nullptr_t {
  auto init = evaluate_expr(var.init);
  _env.define(var.name, init);
  return nullptr;
}

auto Interpreter::execute(const Stmt::Function& fct) -> std::nullptr_t {
  auto& body = const_cast<Stmt::Stmt&>(fct.body);
  auto  obj =
    obj::make<Function>(body, fct.parameters, *this, _env, _env.save_scope());

  _env.define(fct.name, obj);
  return nullptr;
}

auto Interpreter::execute(const Stmt::Struct& strc) -> std::nullptr_t {
  auto& ctor    = const_cast<Expr::Expr&>(strc.ctor);
  auto& dtor    = const_cast<Expr::Expr&>(strc.dtor);
  auto& fields  = const_cast<std::vector<Stmt::Stmt>&>(strc.fields);
  auto& methods = const_cast<std::vector<Stmt::Stmt>&>(strc.methods);

  auto super = obj::make<Vid>();
  if (!strc.parent.empty()) {
    auto super_obj = _env.get(strc.parent);
    obj::cast_to<Struct>(super_obj);
    super = super_obj;
  }

  auto obj = obj::make<Struct>(
    std::string {strc.name},
    super,   //
    ctor,    //
    dtor,    //
    fields,  //
    methods, //
    *this,   //
    _env     //
  );

  _env.define(strc.name, obj);
  return nullptr;
}

auto Interpreter::execute(const Stmt::Loop& l) -> std::nullptr_t {
  while (evaluate_expr(l.clause)->truthy()) {
    try {
      execute_stmt(l.body);
    } catch (Interrupt& interupt) {
      if (interupt.type == Stmt::Interrupt::Type::brk) { break; }
    }
  }

  return nullptr;
}

auto Interpreter::execute(const Stmt::Conditional& cond) -> std::nullptr_t {
  if (evaluate_expr(cond.clause)->truthy()) {
    execute_stmt(cond.true_stmt);
  } else {
    execute_stmt(cond.false_stmt);
  }

  return nullptr;
}

auto Interpreter::execute(const Stmt::Block& blk) -> std::nullptr_t {
  auto scp = _env.new_scope();

  for (auto&& stmt : blk.body) {
    execute_stmt(stmt);
  }

  return nullptr;
}

auto Interpreter::execute(const Stmt::Interrupt& r) -> std::nullptr_t {
  throw Interrupt {
    evaluate_expr(r.ret),
    Stmt::Interrupt::Type::ret,
  };
}

auto Interpreter::execute(const Stmt::ExprStmt& e) -> std::nullptr_t {
  evaluate_expr(e.expr);
  return nullptr;
}

auto Interpreter::has_error() const -> bool {
  return !_errors.empty();
}

auto Interpreter::clear_errors() -> void {
  _errors.clear();
}

auto Interpreter::errors() -> std::vector<RuntimeError>& {
  return _errors;
}

auto Interpreter::interpret(AST& ast, std::istream&, std::ostream&) -> void {
  try {
    ast.execute_with(*this);
  } catch (RuntimeError e) {
    _errors.push_back(e);
    return;
  }
}