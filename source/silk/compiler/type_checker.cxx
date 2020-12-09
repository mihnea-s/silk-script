#include "silk/parser/ast.h"
#include <silk/compiler/type_checker.h>

#include <silk/util/error.h>

auto TypeChecker::evaluate(ASTNode &parent, Identifier &) -> Typing {
  return nullptr;
}

auto TypeChecker::evaluate(ASTNode &parent, Unary &) -> Typing {
  return nullptr;
}

auto TypeChecker::evaluate(ASTNode &parent, Binary &) -> Typing {
  return nullptr;
}

auto TypeChecker::evaluate(ASTNode &parent, KeyLiteral &) -> Typing {
  return nullptr;
}

auto TypeChecker::evaluate(ASTNode &parent, BoolLiteral &) -> Typing {
  return nullptr;
}

auto TypeChecker::evaluate(ASTNode &parent, IntLiteral &) -> Typing {
  return nullptr;
}

auto TypeChecker::evaluate(ASTNode &parent, RealLiteral &) -> Typing {
  return nullptr;
}

auto TypeChecker::evaluate(ASTNode &parent, CharLiteral &) -> Typing {
  return nullptr;
}

auto TypeChecker::evaluate(ASTNode &parent, StringLiteral &) -> Typing {
  return nullptr;
}

auto TypeChecker::evaluate(ASTNode &parent, ArrayLiteral &) -> Typing {
  return nullptr;
}

auto TypeChecker::evaluate(ASTNode &parent, VectorLiteral &) -> Typing {
  return nullptr;
}

auto TypeChecker::evaluate(ASTNode &parent, Lambda &) -> Typing {
  return nullptr;
}

auto TypeChecker::evaluate(ASTNode &parent, Assignment &) -> Typing {
  return nullptr;
}

auto TypeChecker::evaluate(ASTNode &parent, Call &) -> Typing {
  return nullptr;
}

auto TypeChecker::evaluate(ASTNode &parent, Access &) -> Typing {
  return nullptr;
}

auto TypeChecker::execute(Empty &) -> void {
  return;
}

auto TypeChecker::execute(Package &) -> void {
  return;
}

auto TypeChecker::execute(Variable &) -> void {
  return;
}

auto TypeChecker::execute(ExprStmt &) -> void {
  return;
}

auto TypeChecker::execute(Block &) -> void {
  return;
}

auto TypeChecker::execute(Conditional &) -> void {
  return;
}

auto TypeChecker::execute(Loop &) -> void {
  return;
}

auto TypeChecker::execute(Foreach &) -> void {
  return;
}

auto TypeChecker::execute(Match &) -> void {
  return;
}

auto TypeChecker::execute(MatchCase &) -> void {
  return;
}

auto TypeChecker::execute(ControlFlow &) -> void {
  return;
}

auto TypeChecker::execute(Return &) -> void {
  return;
}

auto TypeChecker::execute(Constant &) -> void {
  return;
}

auto TypeChecker::execute(Function &) -> void {
  return;
}

auto TypeChecker::execute(Enum &) -> void {
  return;
}

auto TypeChecker::execute(Struct &) -> void {
  return;
}

auto TypeChecker::execute(Main &) -> void {
  return;
}

auto TypeChecker::type_check(AST &ast) noexcept -> void {
  for (auto &stmt : ast) {
    try {
      TypeChecker::execute_statement(stmt);
    } catch (...) {}
  }
}
