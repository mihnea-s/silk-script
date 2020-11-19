#pragma once

#include <cstddef>
#include <cstdint>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include <silk/lexer/token.h>
#include <silk/util/error.h>

using Typing = std::nullptr_t;

using TypedFields = std::vector<std::pair<std::string, Typing>>;

// Expressions

struct Identifier;
struct Unary;
struct Binary;
struct Constant;
struct BoolLiteral;
struct IntLiteral;
struct RealLiteral;
struct CharLiteral;
struct StringLiteral;
struct ArrayLiteral;
struct Lambda;
struct Assignment;
struct Call;
struct Access;
struct ConstExpr;

// Statements

struct Empty;
struct Package;
struct ExprStmt;
struct Block;
struct Conditional;
struct Loop;
struct Foreach;
struct Match;
struct MatchCase;
struct ControlFlow;
struct Return;
struct Variable;
struct Function;
struct Enum;
struct Struct;

using Expression = std::variant<
  std::unique_ptr<Identifier>,
  std::unique_ptr<Unary>,
  std::unique_ptr<Binary>,
  std::unique_ptr<BoolLiteral>,
  std::unique_ptr<IntLiteral>,
  std::unique_ptr<RealLiteral>,
  std::unique_ptr<CharLiteral>,
  std::unique_ptr<StringLiteral>,
  std::unique_ptr<ArrayLiteral>,
  std::unique_ptr<Constant>,
  std::unique_ptr<Lambda>,
  std::unique_ptr<Assignment>,
  std::unique_ptr<Call>,
  std::unique_ptr<Access>,
  std::unique_ptr<ConstExpr>>;

using Statement = std::variant<
  std::unique_ptr<Empty>,
  std::unique_ptr<Package>,
  std::unique_ptr<ExprStmt>,
  std::unique_ptr<Block>,
  std::unique_ptr<Conditional>,
  std::unique_ptr<Loop>,
  std::unique_ptr<Foreach>,
  std::unique_ptr<Match>,
  std::unique_ptr<MatchCase>,
  std::unique_ptr<ControlFlow>,
  std::unique_ptr<Return>,
  std::unique_ptr<Variable>,
  std::unique_ptr<Function>,
  std::unique_ptr<Enum>,
  std::unique_ptr<Struct>>;

struct ASTNode {
  Location   location;
  Typing     typing;
  Expression expression;
};

using AST = std::vector<Statement>;

struct Identifier {
  enum { REF, VAL } which;
  std::string identifier;
};

struct Unary {
  TokenType operation;
  ASTNode   operand;
};

struct Binary {
  TokenType operation;
  ASTNode   left;
  ASTNode   right;
};

struct BoolLiteral {
  bool value;
};

struct IntLiteral {
  std::int64_t value;
};

struct RealLiteral {
  double value;
};

struct CharLiteral {
  char value;
};

struct StringLiteral {
  std::string value;
};

struct ArrayLiteral {
  std::vector<ASTNode> contents;
};

struct Constant {
  enum { PI, TAU, EULER, VOID } which;
};

struct Lambda {
  Typing      return_type;
  TypedFields parameters;
  Statement   body;
};

struct Assignment {
  ASTNode target;
  ASTNode value;
  enum { ASSIGN, ADD, SUBTRACT } type;
};

struct Call {
  ASTNode              target;
  std::vector<ASTNode> args;
};

struct Access {
  ASTNode target;
  ASTNode property;
};

struct ConstExpr {
  ASTNode inner;
};

// Statements

struct Empty { };

struct Package {
  std::string package;
  enum { DECLARE, IMPORT } action;
};

struct ExprStmt {
  ASTNode node;
};

struct Block {
  std::vector<Statement> body;
};

struct Conditional {
  ASTNode   clause;
  Statement conseq;
  Statement altern;
};

struct Loop {
  ASTNode   clause;
  Statement body;
};

struct Foreach {
  ASTNode   binding;
  ASTNode   range;
  Statement body;
};

struct Match {
  ASTNode                target;
  std::vector<Statement> cases;
};

struct MatchCase {
  // ASTNodePtr pattern;
  Statement body;
};

struct ControlFlow {
  enum { BREAK, CONTINUE } type;
};

struct Return {
  ASTNode value;
};

struct Variable {
  std::string name;
  Typing      typing;
  ASTNode     init;
  bool        immut;
};

struct Function {
  std::string name;
  Lambda      lambda;
};

struct Enum {
  std::string name;
  TypedFields variants;
};

struct Struct {
  std::string           name;
  TypedFields           fields;
  std::vector<Function> methods;
};

// Visitor

template <class E, class S>
class ASTHandler {
  protected:
  virtual E evaluate(Identifier&)    = 0;
  virtual E evaluate(Unary&)         = 0;
  virtual E evaluate(Binary&)        = 0;
  virtual E evaluate(BoolLiteral&)   = 0;
  virtual E evaluate(IntLiteral&)    = 0;
  virtual E evaluate(RealLiteral&)   = 0;
  virtual E evaluate(StringLiteral&) = 0;
  virtual E evaluate(ArrayLiteral&)  = 0;
  virtual E evaluate(Constant&)      = 0;
  virtual E evaluate(Lambda&)        = 0;
  virtual E evaluate(Assignment&)    = 0;
  virtual E evaluate(Call&)          = 0;
  virtual E evaluate(Access&)        = 0;
  virtual E evaluate(ConstExpr&)     = 0;

  virtual S execute(Empty&)       = 0;
  virtual S execute(Package&)     = 0;
  virtual S execute(ExprStmt&)    = 0;
  virtual S execute(Block&)       = 0;
  virtual S execute(Conditional&) = 0;
  virtual S execute(Loop&)        = 0;
  virtual S execute(Foreach&)     = 0;
  virtual S execute(Match&)       = 0;
  virtual S execute(MatchCase&)   = 0;
  virtual S execute(ControlFlow&) = 0;
  virtual S execute(Return&)      = 0;
  virtual S execute(Variable&)    = 0;
  virtual S execute(Function&)    = 0;
  virtual S execute(Struct&)      = 0;

  public:
  E evaluate(Expression& e) {
    return std::visit([&](auto&& expr) { return this->evaluate(*expr); }, e);
  }

  S execute(Statement& s) {
    return std::visit([&](auto&& stmt) { return this->execute(*stmt); }, s);
  }
};