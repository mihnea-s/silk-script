#include <cassert>
#include <regex>
#include <string>

#include <vm/object.h>
#include <vm/value.h>

#include <silk/common/ast.h>
#include <silk/common/error.h>
#include <silk/common/token.h>
#include <silk/debugger/debugger.h>

auto Debugger::Environment::pop() -> void {
  _current = _scopes.top();
  _scopes.pop();
}

auto Debugger::Environment::new_scope() -> ScopeWrapper {
  _scopes.push(_current);
  return ScopeWrapper(*this);
}

auto Debugger::Environment::from_scope(std::shared_ptr<Scope> old)
  -> ScopeWrapper {
  _scopes.push(_current);
  _current = old;
  return ScopeWrapper(*this);
}

auto Debugger::Environment::save_scope() -> std::shared_ptr<Scope> {
  return _current;
}

auto Debugger::Environment::define(const std::string& name, ValuePtr& value)
  -> void {
  auto new_scope = std::make_shared<Scope>(value, name, _current);
  _current       = new_scope;
}

ValuePtr Debugger::Environment::get(const std::string& name) {
  auto searched = _current;

  while (searched && searched->name != name) {
    searched = searched->parent;
  }

  // if (!searched) { throw RuntimeError {"use of undefined variable"}; }

  return searched->value;
}

ValuePtr
Debugger::Environment::assign(const std::string& name, ValuePtr& value) {
  auto searched = _current;

  while (searched->name != name) {
    searched = searched->parent;
  }

  // if (!searched) {
  //   throw RuntimeError {
  //     "assignment of undefined variable",
  //   };
  // }

  searched->value = value;
  return searched->value;
}

inline auto Debugger::string_representation(ValuePtr val) -> std::string {
  return "SOME VALUE!";
}

inline auto Debugger::interpolate_str(std::string str) -> std::string {
  std::ostringstream output {};

  std::regex  var_regex(R"#(\$(\w+)\$)#");
  std::smatch match;

  while (std::regex_search(str, match, var_regex)) {
    assert(match.size() == 2);

    output << match.prefix();
    output << string_representation(_env.get(match[1]));
    str = match.suffix();
  }

  output << str;

  return output.str();
}

inline auto Debugger::shortcircuits(TokenType t) -> bool {
  return t == TokenType::sym_ampamp || t == TokenType::sym_pipepipe;
}

auto Debugger::evaluate(const Unary& expr) -> ValuePtr {
  // auto obj = evaluate_expr(expr.operand);

  // switch (expr.operation.type()) {
  //   case TokenType::sym_bang: {
  //     return obj::make(!obj->truthy());
  //   }

  //   case TokenType::sym_tilde: {
  //     auto& bits = obj::cast_to<BitModifiable>(obj);
  //     return ~bits;
  //   }

  //   case TokenType::sym_minus: {
  //     auto& num = obj::cast_to<Numerical>(obj);
  //     return -num;
  //   };

  //   default: {
  //     throw RuntimeError {"unsupported unary operation"};
  //   }
  // }

  return nullptr;
}

auto Debugger::evaluate(const Binary& expr) -> ValuePtr {
  // auto left_obj = evaluate_expr(expr.left_operand);

  // if (shortcircuits(expr.operation.type())) {

  //   if (expr.operation.type() == TokenType::sym_pipepipe) {
  //     if (left_obj->truthy()) { return obj::make(true); }
  //   } else {
  //     if (!left_obj->truthy()) { return obj::make(false); }
  //   }

  //   return obj::make(evaluate_expr(expr.right_operand)->truthy());
  // }

  // auto right_obj = evaluate_expr(expr.right_operand);

  // switch (expr.operation.type()) {

  //   case TokenType::sym_plus: {
  //     auto& left  = obj::cast_to<Concatenable>(left_obj);
  //     auto& right = obj::cast_to<Concatenable>(right_obj);
  //     return left + right;
  //   }

  //   case TokenType::sym_minus: {
  //     auto& left  = obj::cast_to<Numerical>(left_obj);
  //     auto& right = obj::cast_to<Numerical>(right_obj);
  //     return left - right;
  //   }

  //   case TokenType::sym_star: {
  //     auto& left  = obj::cast_to<Concatenable>(left_obj);
  //     auto& right = obj::cast_to<Concatenable>(right_obj);
  //     return left * right;
  //   }

  //   case TokenType::sym_slash: {
  //     auto& left  = obj::cast_to<Numerical>(left_obj);
  //     auto& right = obj::cast_to<Numerical>(right_obj);
  //     return left / right;
  //   }

  //   case TokenType::sym_starstar: {
  //     auto& left  = obj::cast_to<Numerical>(left_obj);
  //     auto& right = obj::cast_to<Numerical>(right_obj);
  //     return left.starstar(right);
  //   }

  //   case TokenType::sym_slashslash: {
  //     auto& left  = obj::cast_to<Numerical>(left_obj);
  //     auto& right = obj::cast_to<Numerical>(right_obj);
  //     return left.slashslash(right);
  //   }

  //   case TokenType::sym_percent: {
  //     auto& left  = obj::cast_to<BitModifiable>(left_obj);
  //     auto& right = obj::cast_to<BitModifiable>(right_obj);
  //     return left % right;
  //   }

  //   case TokenType::sym_equalequal: {
  //     return *left_obj == right_obj;
  //   }

  //   case TokenType::sym_bangequal: {
  //     return obj::make(!(*left_obj == right_obj)->truthy());
  //   }

  //   case TokenType::sym_gt: {
  //     auto& left  = obj::cast_to<Orderable>(left_obj);
  //     auto& right = obj::cast_to<Orderable>(right_obj);
  //     return left > right;
  //   }

  //   case TokenType::sym_lt: {
  //     auto& left  = obj::cast_to<Orderable>(left_obj);
  //     auto& right = obj::cast_to<Orderable>(right_obj);
  //     return left < right;
  //   }

  //   case TokenType::sym_gtequal: {
  //     auto& left  = obj::cast_to<Orderable>(left_obj);
  //     auto& right = obj::cast_to<Orderable>(right_obj);
  //     return left >= right;
  //   }

  //   case TokenType::sym_ltequal: {
  //     auto& left  = obj::cast_to<Orderable>(left_obj);
  //     auto& right = obj::cast_to<Orderable>(right_obj);
  //     return left <= right;
  //   }

  //   case TokenType::sym_amp: {
  //     auto& left  = obj::cast_to<BitModifiable>(left_obj);
  //     auto& right = obj::cast_to<BitModifiable>(right_obj);
  //     return left & right;
  //   }

  //   case TokenType::sym_pipe: {
  //     auto& left  = obj::cast_to<BitModifiable>(left_obj);
  //     auto& right = obj::cast_to<BitModifiable>(right_obj);
  //     return left | right;
  //   }

  //   case TokenType::sym_caret: {
  //     auto& left  = obj::cast_to<BitModifiable>(left_obj);
  //     auto& right = obj::cast_to<BitModifiable>(right_obj);
  //     return left ^ right;
  //   }

  //   default: {
  //     throw RuntimeError {"unsupported binary operation"};
  //   }
  // }

  return nullptr;
}

auto Debugger::evaluate(const IntLiteral& node) -> ValuePtr {
  return std::make_shared<Value>(INT_VAL(node.value));
}

auto Debugger::evaluate(const RealLiteral& node) -> ValuePtr {
  return std::make_shared<Value>(REAL_VAL(node.value));
}

auto Debugger::evaluate(const BoolLiteral& node) -> ValuePtr {
  return std::make_shared<Value>(BOOL_VAL(node.value));
}

auto Debugger::evaluate(const StringLiteral& node) -> ValuePtr {
  Value value = {
    .type      = T_STR,
    .as.string = const_cast<char*>(node.value.data()),
  };

  return std::make_shared<Value>(value);
}

auto Debugger::evaluate(const Vid&) -> ValuePtr {
  return std::make_shared<Value>(VID_VAL);
}

auto Debugger::evaluate(const Constant& node) -> ValuePtr {
  switch (node.which) {
    case Constant::PI: return std::make_shared<Value>(REAL_VAL(M_PI));
    case Constant::TAU: return std::make_shared<Value>(REAL_VAL(2.0 * M_PI));
    case Constant::EULER: return std::make_shared<Value>(REAL_VAL(M_E));
  }
}

auto Debugger::evaluate(const Assignment& node) -> ValuePtr {
  return nullptr;
}

auto Debugger::evaluate(const Identifier& node) -> ValuePtr {
  return nullptr;
}

auto Debugger::evaluate(const Grouping& node) -> ValuePtr {
  return visit_node(node.inner);
}

auto Debugger::evaluate(const Call& node) -> ValuePtr {
  return nullptr;
}

auto Debugger::evaluate(const Get& node) -> ValuePtr {
  return nullptr;
}

auto Debugger::evaluate(const Lambda& node) -> ValuePtr {
  return nullptr;
}

auto Debugger::execute(const Empty&) -> ValuePtr {
  return nullptr;
}

auto Debugger::execute(const Package& node) -> ValuePtr {
  return nullptr;
}

auto Debugger::execute(const Variable& node) -> ValuePtr {
  // auto init = evaluate_expr(var.init);
  // _env.define(var.name, init);
  return nullptr;
}

auto Debugger::execute(const Function& node) -> ValuePtr {
  // auto& body = const_cast<Stmt&>(fct.body);
  // auto  obj =
  //   obj::make<Function>(body, fct.parameters, *this, _env,
  //   _env.save_scope());

  // _env.define(fct.name, obj);
  return nullptr;
}

auto Debugger::execute(const Struct& node) -> ValuePtr {
  // auto& ctor    = const_cast<Expr&>(strc.ctor);
  // auto& dtor    = const_cast<Expr&>(strc.dtor);
  // auto& fields  = const_cast<std::vector<Stmt>&>(strc.fields);
  // auto& methods = const_cast<std::vector<Stmt>&>(strc.methods);

  // auto super = obj::make<Vid>();
  // if (!strc.parent.empty()) {
  //   auto super_obj = _env.get(strc.parent);
  //   obj::cast_to<Struct>(super_obj);
  //   super = super_obj;
  // }

  // auto obj = obj::make<Struct>(
  //   std::string {strc.name},
  //   super,   //
  //   ctor,    //
  //   dtor,    //
  //   fields,  //
  //   methods, //
  //   *this,   //
  //   _env     //
  // );

  // _env.define(strc.name, obj);
  return nullptr;
}

auto Debugger::execute(const Loop& node) -> ValuePtr {
  // while (evaluate_expr(l.clause)->truthy()) {
  //   try {
  //     execute_stmt(l.body);
  //   } catch (Interrupt& interupt) {
  //     if (interupt.type == Interrupt::Type::brk) { break; }
  //   }
  // }

  return nullptr;
}

auto Debugger::execute(const Conditional& node) -> ValuePtr {
  // if (evaluate_expr(cond.clause)->truthy()) {
  //   execute_stmt(cond.true_stmt);
  // } else {
  //   execute_stmt(cond.false_stmt);
  // }

  return nullptr;
}

auto Debugger::execute(const Block& node) -> ValuePtr {
  // auto scp = _env.new_scope();

  // for (auto&& stmt : blk.body) {
  //   execute_stmt(stmt);
  // }

  return nullptr;
}

auto Debugger::execute(const Interrupt& node) -> ValuePtr {
  // throw Interrupt {
  //   evaluate_expr(r.ret),
  //   Interrupt::Type::ret,
  // };

  return nullptr;
}

auto Debugger::execute(const ExprStmt& node) -> ValuePtr {
  visit_node(node.expression);
  return nullptr;
}

auto Debugger::debug(AST& ast, std::istream&, std::ostream&) -> void {
  try {
    for (const auto& node : ast.program) {
      visit_node(node);
    }
  } catch (...) {}
}