#pragma once

#include <map>

#include <silk/parser/ast.h>
#include <silk/util/error.h>

namespace silk {

/// TODoc
class TypeChecker final :
    public ErrorReporter,
    public ASTHandler<Typing, void> {
private:
  auto evaluate(ASTNode &, Identifier &) -> Typing override;
  auto evaluate(ASTNode &, Unary &) -> Typing override;
  auto evaluate(ASTNode &, Binary &) -> Typing override;
  auto evaluate(ASTNode &, KeyLiteral &) -> Typing override;
  auto evaluate(ASTNode &, BoolLiteral &) -> Typing override;
  auto evaluate(ASTNode &, IntLiteral &) -> Typing override;
  auto evaluate(ASTNode &, RealLiteral &) -> Typing override;
  auto evaluate(ASTNode &, CharLiteral &) -> Typing override;
  auto evaluate(ASTNode &, StringLiteral &) -> Typing override;
  auto evaluate(ASTNode &, VectorLiteral &) -> Typing override;
  auto evaluate(ASTNode &, ArrayLiteral &) -> Typing override;
  auto evaluate(ASTNode &, DictionaryLiteral &) -> Typing override;
  auto evaluate(ASTNode &, Assignment &) -> Typing override;
  auto evaluate(ASTNode &, Lambda &) -> Typing override;
  auto evaluate(ASTNode &, Call &) -> Typing override;

  auto execute(Empty &) -> void override;
  auto execute(Package &) -> void override;
  auto execute(Variable &) -> void override;
  auto execute(ExprStmt &) -> void override;
  auto execute(Block &) -> void override;
  auto execute(Conditional &) -> void override;
  auto execute(Loop &) -> void override;
  auto execute(Foreach &) -> void override;
  auto execute(Match &) -> void override;
  auto execute(MatchCase &) -> void override;
  auto execute(ControlFlow &) -> void override;
  auto execute(Return &) -> void override;
  auto execute(Constant &) -> void override;
  auto execute(Function &) -> void override;
  auto execute(Enum &) -> void override;
  auto execute(Object &) -> void override;
  auto execute(Main &) -> void override;

public:
  TypeChecker() {
  }

  ~TypeChecker() {
  }

  /// TODoc
  auto type_check(AST &ast) noexcept -> void;
};

} // namespace silk
