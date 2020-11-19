#pragma once

#include <map>

#include <silk/parser/ast.h>
#include <silk/util/error.h>

class TypeChecker : public ErrorReporter, public ASTHandler<Typing, void> {
  private:
  auto evaluate(Identifier&) -> Typing final;
  auto evaluate(Unary&) -> Typing final;
  auto evaluate(Binary&) -> Typing final;
  auto evaluate(BoolLiteral&) -> Typing final;
  auto evaluate(IntLiteral&) -> Typing final;
  auto evaluate(RealLiteral&) -> Typing final;
  auto evaluate(StringLiteral&) -> Typing final;
  auto evaluate(ArrayLiteral&) -> Typing final;
  auto evaluate(Constant&) -> Typing final;
  auto evaluate(Lambda&) -> Typing final;
  auto evaluate(Assignment&) -> Typing final;
  auto evaluate(Call&) -> Typing final;
  auto evaluate(Access&) -> Typing final;
  auto evaluate(ConstExpr&) -> Typing final;

  auto execute(Empty&) -> void final;
  auto execute(Package&) -> void final;
  auto execute(ExprStmt&) -> void final;
  auto execute(Block&) -> void final;
  auto execute(Conditional&) -> void final;
  auto execute(Loop&) -> void final;
  auto execute(Foreach&) -> void final;
  auto execute(Match&) -> void final;
  auto execute(MatchCase&) -> void final;
  auto execute(ControlFlow&) -> void final;
  auto execute(Return&) -> void final;
  auto execute(Variable&) -> void final;
  auto execute(Function&) -> void final;
  auto execute(Struct&) -> void final;

  public:
  // check ast function
  auto type_check(AST& ast) noexcept -> void;
};