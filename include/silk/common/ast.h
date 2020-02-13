#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "token.h"

struct ASTNode {
  enum NodeType {
    Unary,
    Binary,
    IntLiteral,
    RealLiteral,
    StringLiteral,
    BoolLiteral,
    Vid,
    Constant,
    Lambda,
    Identifier,
    Assignment,
    Grouping,
    Call,
    Get,

    Empty,
    Package,
    ExprStmt,
    Variable,
    Function,
    Struct,
    Loop,
    Conditional,
    Block,
    Interrupt,
  };

  using Location = std::pair<std::uint64_t, std::uint64_t>;
  const Location location;
  const NodeType type;
};

using ASTNodePtr    = std::unique_ptr<ASTNode>;
using ASTNodeList   = std::vector<ASTNodePtr>;
using ASTType       = std::string;              // TODO
using ASTParameters = std::vector<std::string>; // TODO

struct AST {
  const ASTNodeList program;
};

// Expressions

struct Unary : ASTNode {
  constexpr static auto node_enum = ASTNode::Unary;
  const TokenType       operation;
  const ASTNodePtr      operand;
};

struct Binary : ASTNode {
  constexpr static auto node_enum = ASTNode::Binary;
  const TokenType       operation;
  const ASTNodePtr      left;
  const ASTNodePtr      right;
};

struct IntLiteral : ASTNode {
  constexpr static auto node_enum = ASTNode::IntLiteral;
  const std::int64_t    value;
};

struct RealLiteral : ASTNode {
  constexpr static auto node_enum = ASTNode::RealLiteral;
  const double          value;
};

struct StringLiteral : ASTNode {
  constexpr static auto node_enum = ASTNode::StringLiteral;
  const std::string     value;
};

struct BoolLiteral : ASTNode {
  constexpr static auto node_enum = ASTNode::BoolLiteral;
  const bool            value;
};

struct Vid : ASTNode {
  constexpr static auto node_enum = ASTNode::Vid;
};

struct Constant : ASTNode {
  constexpr static auto node_enum = ASTNode::Constant;
  enum Which { PI, TAU, EULER };
  const Which which;
};

struct Lambda : ASTNode {
  constexpr static auto node_enum = ASTNode::Lambda;
  const ASTType         return_type;
  const ASTParameters   parameters;
  const ASTNodePtr      body;
};

struct Identifier : ASTNode {
  constexpr static auto node_enum = ASTNode::Identifier;
  const std::string     identifier;
};

struct Assignment : ASTNode {
  constexpr static auto node_enum = ASTNode::Assignment;
  const ASTNodePtr      target;
  const ASTNodePtr      assignment;
};

struct Grouping : ASTNode {
  constexpr static auto node_enum = ASTNode::Grouping;
  const ASTNodePtr      inner;
};

struct Call : ASTNode {
  constexpr static auto node_enum = ASTNode::Call;
  const ASTNodePtr      target;
  const ASTNodeList     args;
};

struct Get : ASTNode {
  constexpr static auto node_enum = ASTNode::Get;
  const ASTNodePtr      target;
  const ASTNodePtr      property;
};

// Statements

struct Empty : ASTNode {
  constexpr static auto node_enum = ASTNode::Empty;
};

struct Package : ASTNode {
  constexpr static auto node_enum = ASTNode::Package;
  enum PackageAction { DECLARATION, IMPORT };
  const std::string   package;
  const PackageAction action;
};

struct ExprStmt : ASTNode {
  constexpr static auto node_enum = ASTNode::ExprStmt;
  const ASTNodePtr      expression;
};

struct Loop : ASTNode {
  constexpr static auto node_enum = ASTNode::Loop;
  const ASTNodePtr      clause;
  const ASTNodePtr      body;
};

struct Conditional : ASTNode {
  constexpr static auto node_enum = ASTNode::Conditional;
  const ASTNodePtr      clause;
  const ASTNodePtr      if_true;
  const ASTNodePtr      if_false;
};

struct Block : ASTNode {
  constexpr static auto node_enum = ASTNode::Block;
  const ASTNodeList     body;
};

struct Interrupt : ASTNode {
  constexpr static auto node_enum = ASTNode::Interrupt;
  enum InterruptType { RETURN, BREAK, CONTINUE };
  const ASTNodePtr    payload;
  const InterruptType type;
};

struct Variable : ASTNode {
  constexpr static auto node_enum = ASTNode::Variable;
  const std::string     name;
  const ASTType         type;
  const ASTNodePtr      initializer;
};

struct Function : ASTNode {
  constexpr static auto node_enum = ASTNode::Function;
  const std::string     name;
  const ASTType         return_type;
  const ASTParameters   parameters;
  const ASTNodePtr      body;
  const bool            is_virtual;
};

struct Struct : ASTNode {
  constexpr static auto node_enum = ASTNode::Struct;
  const std::string     name;
  const ASTType         parent;

  const ASTNodePtr constructor;
  const ASTNodePtr destructor;

  const ASTNodeList fields;
  const ASTNodeList methods;
};

// Visitor

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
  virtual T evaluate(const Identifier&)    = 0;
  virtual T evaluate(const Assignment&)    = 0;
  virtual T evaluate(const Grouping&)      = 0;
  virtual T evaluate(const Call&)          = 0;
  virtual T evaluate(const Get&)           = 0;

  virtual T execute(const Empty&)       = 0;
  virtual T execute(const Package&)     = 0;
  virtual T execute(const ExprStmt&)    = 0;
  virtual T execute(const Variable&)    = 0;
  virtual T execute(const Function&)    = 0;
  virtual T execute(const Struct&)      = 0;
  virtual T execute(const Loop&)        = 0;
  virtual T execute(const Conditional&) = 0;
  virtual T execute(const Block&)       = 0;
  virtual T execute(const Interrupt&)   = 0;

  T visit_node(const ASTNodePtr& node) {
    auto& node_ref = *node.get();
    switch (node->type) {
      case ASTNode::Unary: //
        return evaluate(static_cast<const Unary&>(node_ref));
      case ASTNode::Binary: //
        return evaluate(static_cast<const Binary&>(node_ref));
      case ASTNode::IntLiteral: //
        return evaluate(static_cast<const IntLiteral&>(node_ref));
      case ASTNode::RealLiteral: //
        return evaluate(static_cast<const RealLiteral&>(node_ref));
      case ASTNode::BoolLiteral: //
        return evaluate(static_cast<const BoolLiteral&>(node_ref));
      case ASTNode::StringLiteral: //
        return evaluate(static_cast<const StringLiteral&>(node_ref));
      case ASTNode::Vid: //
        return evaluate(static_cast<const Vid&>(node_ref));
      case ASTNode::Constant:
        return evaluate(static_cast<const Constant&>(node_ref));
      case ASTNode::Lambda: //
        return evaluate(static_cast<const Lambda&>(node_ref));
      case ASTNode::Identifier: //
        return evaluate(static_cast<const Identifier&>(node_ref));
      case ASTNode::Assignment: //
        return evaluate(static_cast<const Assignment&>(node_ref));
      case ASTNode::Grouping: //
        return evaluate(static_cast<const Grouping&>(node_ref));
      case ASTNode::Call: //
        return evaluate(static_cast<const Call&>(node_ref));
      case ASTNode::Get: //
        return evaluate(static_cast<const Get&>(node_ref));

      case ASTNode::Empty: //
        return execute(static_cast<const Empty&>(node_ref));
      case ASTNode::Package: //
        return execute(static_cast<const Package&>(node_ref));
      case ASTNode::ExprStmt: //
        return execute(static_cast<const ExprStmt&>(node_ref));
      case ASTNode::Variable: //
        return execute(static_cast<const Variable&>(node_ref));
      case ASTNode::Function: //
        return execute(static_cast<const Function&>(node_ref));
      case ASTNode::Struct: //
        return execute(static_cast<const Struct&>(node_ref));
      case ASTNode::Loop: //
        return execute(static_cast<const Loop&>(node_ref));
      case ASTNode::Conditional: //
        return execute(static_cast<const Conditional&>(node_ref));
      case ASTNode::Block: //
        return execute(static_cast<const Block&>(node_ref));
      case ASTNode::Interrupt: //
        return execute(static_cast<const Interrupt&>(node_ref));
    }
  }
};