#include <silk/compiler/type_checker.h>

#include <silk/util/error.h>

auto evaluate(Identifier&) -> Typing {
  ;
}

auto evaluate(Unary&) -> Typing {
  ;
}

auto evaluate(Binary&) -> Typing {
  ;
}

auto evaluate(BoolLiteral&) -> Typing {
  ;
}

auto evaluate(IntLiteral&) -> Typing {
  ;
}

auto evaluate(RealLiteral&) -> Typing {
  ;
}

auto evaluate(StringLiteral&) -> Typing {
  ;
}

auto evaluate(Constant&) -> Typing {
  ;
}

auto evaluate(Lambda&) -> Typing {
  ;
}

auto evaluate(Assignment&) -> Typing {
  ;
}

auto evaluate(Call&) -> Typing {
  ;
}

auto evaluate(Access&) -> Typing {
  ;
}

auto evaluate(ConstExpr&) -> Typing {
  ;
}

auto execute(Empty&) -> void {
  ;
}

auto execute(Package&) -> void {
  ;
}

auto execute(ExprStmt&) -> void {
  ;
}

auto execute(Block&) -> void {
  ;
}

auto execute(Conditional&) -> void {
  ;
}

auto execute(Loop&) -> void {
  ;
}

auto execute(Foreach&) -> void {
  ;
}

auto execute(Match&) -> void {
  ;
}

auto execute(MatchCase&) -> void {
  ;
}

auto execute(ControlFlow&) -> void {
  ;
}

auto execute(Return&) -> void {
  ;
}

auto execute(Variable&) -> void {
  ;
}

auto execute(Function&) -> void {
  ;
}

auto execute(Struct&) -> void {
  ;
}

auto TypeChecker::type_check(AST& ast) noexcept -> void {
  for (auto& stmt : ast) {
    try {
      execute(stmt);
    } catch (...) { }
  }
}
