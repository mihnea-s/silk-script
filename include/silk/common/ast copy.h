#pragma once

#include <cstddef>
#include <cstdint>
#include <exception>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "token.h"

struct ASTNode;
using ASTNodePtr    = std::unique_ptr<ASTNode>;
using ASTNodeList   = std::vector<ASTNodePtr>;
using ASTType       = std::string;              // TODO
using ASTParameters = std::vector<std::string>; // TODO

struct AST {
  const ASTNodeList program;
};

// Expressions

struct Unary {
  const TokenType  operation;
  const ASTNodePtr operand;
};

struct Binary {
  const TokenType  operation;
  const ASTNodePtr left;
  const ASTNodePtr right;
};

struct IntLiteral {
  const std::int64_t value;
};

struct RealLiteral {
  const double value;
};

struct StringLiteral {
  const std::string value;
};

struct BoolLiteral {
  const bool value;
};

struct Vid {};

struct Constant {
  enum Which { PI, TAU, EULER };
  const Which which;
};

struct Lambda {
  const ASTType       return_type;
  const ASTParameters parameters;
  const ASTNodePtr    body;
};

struct IdentifierRef {
  const std::string identifier;
};

struct IdentifierVal {
  const std::string identifier;
};

struct Assignment {
  const ASTNodePtr target;
  const ASTNodePtr assignment;
};

struct Grouping {
  const ASTNodePtr inner;
};

struct Call {
  const ASTNodePtr  target;
  const ASTNodeList args;
};

struct Access {
  const ASTNodePtr target;
  const ASTNodePtr property;
};

// Statements

struct Empty {};

struct Package {
  enum PackageAction { DECLARATION, IMPORT };
  const std::string   package;
  const PackageAction action;
};

struct ExprStmt {
  const ASTNodePtr expression;
};

struct Loop {
  const ASTNodePtr clause;
  const ASTNodePtr body;
};

struct Conditional {
  const ASTNodePtr clause;
  const ASTNodePtr if_true;
  const ASTNodePtr if_false;
};

struct Match {
  const ASTNodePtr  target;
  const ASTNodeList cases;
};

struct MatchCase {
  const ASTNodePtr pattern;
  const ASTNodePtr body;
};

struct Block {
  const ASTNodeList body;
};

struct Interrupt {
  enum InterruptType { RETURN, BREAK, CONTINUE };
  const ASTNodePtr    payload;
  const InterruptType type;
};

struct Variable {
  const std::string name;
  const ASTType     type;
  const ASTNodePtr  initializer;
};

struct Function {
  const std::string   name;
  const ASTType       return_type;
  const ASTParameters parameters;
  const ASTNodePtr    body;
  const bool          is_virtual;
  const bool          is_async;
};

struct Struct {
  const std::string name;
  const ASTType     parent;

  const ASTNodePtr constructor;
  const ASTNodePtr destructor;

  const ASTNodeList fields;
  const ASTNodeList methods;
};

using ASTVariant = std::variant<
  Unary,
  Binary,
  IntLiteral,
  RealLiteral,
  BoolLiteral,
  StringLiteral,
  Vid,
  Constant,
  Lambda,
  IdentifierRef,
  IdentifierVal,
  Assignment,
  Grouping,
  Call,
  Access>;

struct ASTNode {
  using Location = std::pair<std::uint64_t, std::uint64_t>;
  const Location location;
  const ASTVariant node;

  // ASTNode(Location loc, ASTVariant&& var) :
  //     location(loc), ASTVariant(std::move(var)) {
  // }
};

// Visitor

const ASTNode nis = {
  {0, 2},
  Vid{},
};

template <class T>
class ASTVisitor {
  protected:
  virtual T evaluate(const Unary&)         = 0;
  virtual T evaluate(const Binary&)        = 0;
  virtual T evaluate(const IntLiteral&)    = 0;
  virtual T evaluate(const RealLiteral&)   = 0;
  virtual T evaluate(const BoolLiteral&)   = 0;
  virtual T evaluate(const StringLiteral&) = 0;
  virtual T evaluate(const Vid&)           = 0;
  virtual T evaluate(const Constant&)      = 0;
  virtual T evaluate(const Lambda&)        = 0;
  virtual T evaluate(const IdentifierRef&) = 0;
  virtual T evaluate(const IdentifierVal&) = 0;
  virtual T evaluate(const Assignment&)    = 0;
  virtual T evaluate(const Grouping&)      = 0;
  virtual T evaluate(const Call&)          = 0;
  virtual T evaluate(const Access&)        = 0;

  virtual T execute(const Empty&)       = 0;
  virtual T execute(const Package&)     = 0;
  virtual T execute(const ExprStmt&)    = 0;
  virtual T execute(const Variable&)    = 0;
  virtual T execute(const Function&)    = 0;
  virtual T execute(const Struct&)      = 0;
  virtual T execute(const Loop&)        = 0;
  virtual T execute(const Match&)       = 0;
  virtual T execute(const MatchCase&)   = 0;
  virtual T execute(const Conditional&) = 0;
  virtual T execute(const Block&)       = 0;
  virtual T execute(const Interrupt&)   = 0;

  T visit_node(const ASTNodePtr& node) {
  }
};