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

#include <silk/parser/token.h>
#include <silk/util/error.h>

namespace silk {

using Typing = std::nullptr_t;

using TypedFields = std::vector<std::pair<std::string, Typing>>;

// Expressions

struct Identifier;
struct Unary;
struct Binary;
struct KeyLiteral;
struct BoolLiteral;
struct IntLiteral;
struct RealLiteral;
struct CharLiteral;
struct StringLiteral;
struct ArrayLiteral;
struct VectorLiteral;
struct DictionaryLiteral;
struct Lambda;
struct Assignment;
struct Call;

// Statements

struct Empty;
struct Package;
struct Variable;
struct ExprStmt;
struct Block;
struct Conditional;
struct Loop;
struct Foreach;
struct Match;
struct MatchCase;
struct ControlFlow;
struct Return;
struct Constant;
struct Function;
struct Enum;
struct Object;
struct Main;

using Expression = std::variant<
  std::unique_ptr<Identifier>,
  std::unique_ptr<Unary>,
  std::unique_ptr<Binary>,
  std::unique_ptr<KeyLiteral>,
  std::unique_ptr<BoolLiteral>,
  std::unique_ptr<IntLiteral>,
  std::unique_ptr<RealLiteral>,
  std::unique_ptr<CharLiteral>,
  std::unique_ptr<StringLiteral>,
  std::unique_ptr<VectorLiteral>,
  std::unique_ptr<ArrayLiteral>,
  std::unique_ptr<DictionaryLiteral>,
  std::unique_ptr<Assignment>,
  std::unique_ptr<Lambda>,
  std::unique_ptr<Call>>;

using Statement = std::variant<
  std::unique_ptr<Empty>,
  std::unique_ptr<Package>,
  std::unique_ptr<Variable>,
  std::unique_ptr<ExprStmt>,
  std::unique_ptr<Block>,
  std::unique_ptr<Conditional>,
  std::unique_ptr<Loop>,
  std::unique_ptr<Foreach>,
  std::unique_ptr<Match>,
  std::unique_ptr<MatchCase>,
  std::unique_ptr<ControlFlow>,
  std::unique_ptr<Return>,
  std::unique_ptr<Constant>,
  std::unique_ptr<Function>,
  std::unique_ptr<Enum>,
  std::unique_ptr<Object>,
  std::unique_ptr<Main>>;

struct ASTNode {
private:
  Location   _location;
  Typing     _typing;
  Expression _inner;

public:
  ASTNode(const Location &location, Typing &&typing, Expression &&inner) :
      _location(location), _typing(typing), _inner(std::move(inner)) {
  }

  ASTNode(const ASTNode &) = delete;
  ASTNode(ASTNode &&)      = default;

  auto operator=(const ASTNode &) -> ASTNode & = delete;
  auto operator=(ASTNode &&) -> ASTNode & = default;

  inline auto location() const -> const Location & {
    return _location;
  }
  inline auto typing() -> Typing & {
    return _typing;
  }
  inline auto inner() -> Expression & {
    return _inner;
  }
};

using AST = std::vector<Statement>;

/// TODoc
struct Identifier {
public:
private:
  std::string _identifier;

public:
  Identifier(std::string &&identifier) : _identifier(identifier) {
  }

  Identifier(const Identifier &) = delete;
  Identifier(Identifier &&)      = default;

  inline auto identifier() const -> const std::string & {
    return _identifier;
  }
};

/// TODoc
struct Unary {
private:
  TokenKind _operation;
  ASTNode   _operand;

public:
  Unary(TokenKind operation, ASTNode &&operand) :
      _operation(operation), _operand(std::move(operand)) {
  }

  Unary(const Unary &) = delete;
  Unary(Unary &&)      = default;

  inline auto operation() const -> const TokenKind & {
    return _operation;
  }
  inline auto operand() -> ASTNode & {
    return _operand;
  }
};

/// TODoc
struct Binary {
private:
  TokenKind _operation;
  ASTNode   _left;
  ASTNode   _right;

public:
  Binary(TokenKind operation, ASTNode &&left, ASTNode &&right) :
      _operation(operation), _left(std::move(left)), _right(std::move(right)) {
  }

  Binary(const Binary &) = delete;
  Binary(Binary &&)      = default;

  inline auto operation() const -> const TokenKind & {
    return _operation;
  }
  inline auto left() -> ASTNode & {
    return _left;
  }
  inline auto right() -> ASTNode & {
    return _right;
  }
};

/// TODoc
struct KeyLiteral {
public:
  enum KeyType { VOID, PI, TAU, EULER };

private:
  KeyType _type;

public:
  KeyLiteral(KeyType type) : _type(type) {
  }

  KeyLiteral(const KeyLiteral &) = delete;
  KeyLiteral(KeyLiteral &&)      = default;

  inline auto type() const -> const KeyType & {
    return _type;
  }
};

/// TODoc
struct BoolLiteral {
private:
  bool _value;

public:
  BoolLiteral(bool value) : _value(value) {
  }

  BoolLiteral(const BoolLiteral &) = delete;
  BoolLiteral(BoolLiteral &&)      = default;

  inline auto value() const -> const bool & {
    return _value;
  }
};

/// TODoc
struct IntLiteral {
private:
  std::int64_t _value;

public:
  IntLiteral(std::int64_t value) : _value(value) {
  }

  IntLiteral(const IntLiteral &) = delete;
  IntLiteral(IntLiteral &&)      = default;

  inline auto value() const -> const std::int64_t & {
    return _value;
  }
};

/// TODoc
struct RealLiteral {
private:
  double _value;

public:
  RealLiteral(double value) : _value(value) {
  }

  RealLiteral(const RealLiteral &) = delete;
  RealLiteral(RealLiteral &&)      = default;

  inline auto value() const -> const double & {
    return _value;
  }
};

/// TODoc
struct CharLiteral {
private:
  char32_t _value;

public:
  CharLiteral(char32_t value) : _value(value) {
  }

  CharLiteral(const CharLiteral &) = delete;
  CharLiteral(CharLiteral &&)      = default;

  inline auto value() const -> const char32_t & {
    return _value;
  }
};

/// TODoc
struct StringLiteral {
private:
  std::string _value;

public:
  StringLiteral(std::string &&value) : _value(value) {
  }

  StringLiteral(const StringLiteral &) = delete;
  StringLiteral(StringLiteral &&)      = default;

  inline auto value() const -> const std::string & {
    return _value;
  }
};

/// TODoc
struct VectorLiteral {
private:
  std::vector<ASTNode> _contents;

public:
  VectorLiteral(std::vector<ASTNode> &&contents) :
      _contents(std::move(contents)) {
  }

  VectorLiteral(const VectorLiteral &) = delete;
  VectorLiteral(VectorLiteral &&)      = default;

  inline auto contents() -> std::vector<ASTNode> & {
    return _contents;
  }
};

/// TODoc
struct ArrayLiteral {
private:
  std::vector<ASTNode> _contents;

public:
  ArrayLiteral(std::vector<ASTNode> &&contents) :
      _contents(std::move(contents)) {
  }

  ArrayLiteral(const ArrayLiteral &) = delete;
  ArrayLiteral(ArrayLiteral &&)      = default;

  inline auto contents() -> std::vector<ASTNode> & {
    return _contents;
  }
};

/// TODoc
struct DictionaryLiteral {
private:
  std::vector<std::pair<ASTNode, ASTNode>> _contents;

public:
  DictionaryLiteral(std::vector<std::pair<ASTNode, ASTNode>> &&contents) :
      _contents(std::move(contents)) {
  }

  DictionaryLiteral(const DictionaryLiteral &) = delete;
  DictionaryLiteral(DictionaryLiteral &&)      = default;

  inline auto contents() -> std::vector<std::pair<ASTNode, ASTNode>> & {
    return _contents;
  }
};

/// TODoc
struct Assignment {
private:
  TokenKind _operation;
  ASTNode   _target;
  ASTNode   _value;

public:
  Assignment(TokenKind operation, ASTNode &&target, ASTNode &&value) :
      _operation(operation),
      _target(std::move(target)),
      _value(std::move(value)) {
  }

  Assignment(const Assignment &) = delete;
  Assignment(Assignment &&)      = default;

  inline auto operation() const -> const TokenKind & {
    return _operation;
  }
  inline auto target() -> ASTNode & {
    return _target;
  }
  inline auto value() -> ASTNode & {
    return _value;
  }
};

/// TODoc
struct Lambda {
private:
  Typing      _return_type;
  TypedFields _parameters;
  Statement   _body;

public:
  Lambda(Typing return_type, TypedFields &&parameters, Statement &&body) :
      _return_type(return_type),
      _parameters(std::move(parameters)),
      _body(std::move(body)) {
  }

  Lambda(const Lambda &) = delete;
  Lambda(Lambda &&)      = default;

  inline auto return_type() -> Typing & {
    return _return_type;
  }
  inline auto parameters() -> TypedFields & {
    return _parameters;
  }
  inline auto body() -> Statement & {
    return _body;
  }
};

/// TODoc
struct Call {
private:
  ASTNode              _target;
  std::vector<ASTNode> _args;

public:
  Call(ASTNode &&target, std::vector<ASTNode> &&args) :
      _target(std::move(target)), _args(std::move(args)) {
  }

  Call(const Call &) = delete;
  Call(Call &&)      = default;

  inline auto target() -> ASTNode & {
    return _target;
  }
  inline auto args() -> std::vector<ASTNode> & {
    return _args;
  }
};

// Statements

/// TODoc
struct Empty {
public:
  Empty() {
  }

  Empty(const Empty &) = delete;
  Empty(Empty &&)      = default;
};

struct Package {
public:
  enum PackageType { DECLARE, IMPORT };

private:
  std::string _package;
  PackageType _action;

public:
  Package(std::string &&package, PackageType action) :
      _package(package), _action(action){};

  Package(const Package &) = delete;
  Package(Package &&)      = default;

  inline auto package() const -> const std::string & {
    return _package;
  }
  inline auto action() const -> const PackageType & {
    return _action;
  }
};

/// TODoc
struct Variable {
private:
  std::string _name;
  Typing      _typing;
  ASTNode     _init;
  bool        _immut;

public:
  Variable(std::string &&name, Typing typing, ASTNode &&init, bool immut) :
      _name(std::move(name)),
      _typing(typing),
      _init(std::move(init)),
      _immut(immut) {
  }

  Variable(const Variable &) = delete;
  Variable(Variable &&)      = default;

  inline auto name() const -> const std::string & {
    return _name;
  };
  inline auto typing() -> Typing & {
    return _typing;
  }
  inline auto init() -> ASTNode & {
    return _init;
  }
  inline auto immut() const -> const bool & {
    return _immut;
  }
};

/// TODoc
struct ExprStmt {
private:
  ASTNode _expr;

public:
  ExprStmt(ASTNode &&expr) : _expr(std::move(expr)) {
  }

  ExprStmt(const ExprStmt &) = delete;
  ExprStmt(ExprStmt &&)      = default;

  auto expr() -> ASTNode & {
    return _expr;
  }
};

/// TODoc
struct Block {
private:
  std::vector<Statement> _statements;

public:
  Block(std::vector<Statement> &&statements) :
      _statements(std::move(statements)) {
  }

  Block(const Block &) = delete;
  Block(Block &&)      = default;

  auto statements() -> std::vector<Statement> & {
    return _statements;
  }
};

/// TODoc
struct Conditional {
private:
  ASTNode   _clause;
  Statement _conseq;
  Statement _altern;

public:
  Conditional(ASTNode &&clause, Statement &&conseq, Statement &&altern) :
      _clause(std::move(clause)),
      _conseq(std::move(conseq)),
      _altern(std::move(altern)) {
  }

  Conditional(const Conditional &) = delete;
  Conditional(Conditional &&)      = default;

  inline auto clause() -> ASTNode & {
    return _clause;
  }
  inline auto conseq() -> Statement & {
    return _conseq;
  }
  inline auto altern() -> Statement & {
    return _altern;
  }
};

/// TODoc
struct Loop {
private:
  ASTNode   _clause;
  Statement _body;

public:
  Loop(ASTNode &&clause, Statement &&body) :
      _clause(std::move(clause)), _body(std::move(body)) {
  }

  Loop(const Loop &) = delete;
  Loop(Loop &&)      = default;

  inline auto clause() -> ASTNode & {
    return _clause;
  }
  inline auto body() -> Statement & {
    return _body;
  }
};

/// TODoc
struct Foreach {
private:
  ASTNode   _binding;
  ASTNode   _range;
  Statement _body;

public:
  Foreach(ASTNode &&binding, ASTNode &&range, Statement &&body) :
      _binding(std::move(binding)),
      _range(std::move(range)),
      _body(std::move(body)) {
  }

  Foreach(const Foreach &) = delete;
  Foreach(Foreach &&)      = default;

  inline auto binding() -> ASTNode & {
    return _binding;
  }
  inline auto range() -> ASTNode & {
    return _range;
  }
  inline auto body() -> Statement & {
    return _body;
  }
};

/// TODoc
struct Match {
private:
  ASTNode                _target;
  std::vector<Statement> _cases;

public:
  Match(ASTNode &&target, std::vector<Statement> &&cases) :
      _target(std::move(target)), _cases(std::move(cases)) {
  }

  Match(const Match &) = delete;
  Match(Match &&)      = default;

  inline auto target() -> ASTNode & {
    return _target;
  }
  inline auto cases() -> std::vector<Statement> & {
    return _cases;
  }
};

/// TODoc
struct MatchCase {
private:
  ASTNode   _expr;
  Statement _body;

public:
  MatchCase(ASTNode &&expr, Statement &&body) :
      _expr(std::move(expr)), _body(std::move(body)) {
  }

  MatchCase(const MatchCase &) = delete;
  MatchCase(MatchCase &&)      = default;

  inline auto expr() -> ASTNode & {
    return _expr;
  }
  inline auto body() -> Statement & {
    return _body;
  }
};

/// TODoc
struct ControlFlow {
public:
  enum ControlFlowType { BREAK, CONTINUE };

private:
  ControlFlowType _type;

public:
  ControlFlow(ControlFlowType type) : _type(type) {
  }

  ControlFlow(const ControlFlow &) = delete;
  ControlFlow(ControlFlow &&)      = default;

  inline auto type() const -> const ControlFlowType & {
    return _type;
  }
};

/// TODoc
struct Return {
private:
  ASTNode _value;

public:
  Return(ASTNode &&value) : _value(std::move(value)) {
  }

  Return(const Return &) = delete;
  Return(Return &&)      = default;

  inline auto value() -> ASTNode & {
    return _value;
  }
};

/// TODoc
struct Constant {
private:
  std::string _name;
  Typing      _typing;
  ASTNode     _value;

public:
  Constant(std::string &&name, Typing typing, ASTNode &&value) :
      _name(std::move(name)), _typing(typing), _value(std::move(value)) {
  }

  Constant(const Constant &) = delete;
  Constant(Constant &&)      = default;

  inline auto name() const -> const std::string & {
    return _name;
  }
  inline auto typing() -> Typing & {
    return _typing;
  }
  inline auto value() -> ASTNode & {
    return _value;
  }
};

/// TODoc
struct Function {
private:
  std::string _name;
  ASTNode     _lambda;

public:
  Function(std::string &&name, ASTNode &&lambda) :
      _name(std::move(name)), _lambda(std::move(lambda)) {
  }

  Function(const Function &) = delete;
  Function(Function &&)      = default;

  inline auto name() const -> const std::string & {
    return _name;
  }
  inline auto lambda() -> ASTNode & {
    return _lambda;
  }
};

/// TODoc
struct Object {
private:
  std::string           _name;
  TypedFields           _fields;
  std::vector<Lambda>   _ctors;
  std::vector<Function> _methods;

public:
  Object(
    std::string &&name, TypedFields &&fields, std::vector<Function> &&methods) :
      _name(std::move(name)),
      _fields(std::move(fields)),
      _methods(std::move(methods)) {
  }

  Object(const Object &) = delete;
  Object(Object &&)      = default;

  inline auto name() const -> const std::string & {
    return _name;
  }
  inline auto fields() -> TypedFields & {
    return _fields;
  }
  inline auto methods() -> std::vector<Function> & {
    return _methods;
  }
};

/// TODoc
struct Enum {
private:
  std::string _name;
  TypedFields _variants;

public:
  Enum(std::string &&name, TypedFields &&variants) :
      _name(std::move(name)), _variants(std::move(variants)) {
  }

  Enum(const Enum &) = delete;
  Enum(Enum &&)      = default;

  inline auto name() const -> const std::string & {
    return _name;
  }
  inline auto variants() -> TypedFields & {
    return _variants;
  }
};

/// TODoc
struct Main {
private:
  std::vector<Statement> _body;

public:
  Main(std::vector<Statement> &&body) : _body(std::move(body)) {
  }

  Main(const Main &) = delete;
  Main(Main &&)      = default;

  inline auto body() -> std::vector<Statement> & {
    return _body;
  }
};

// Visitor

/// TODoc
template <class E, class S>
class ASTHandler {
protected:
  virtual E evaluate(ASTNode &, Identifier &)        = 0;
  virtual E evaluate(ASTNode &, Unary &)             = 0;
  virtual E evaluate(ASTNode &, Binary &)            = 0;
  virtual E evaluate(ASTNode &, KeyLiteral &)        = 0;
  virtual E evaluate(ASTNode &, BoolLiteral &)       = 0;
  virtual E evaluate(ASTNode &, IntLiteral &)        = 0;
  virtual E evaluate(ASTNode &, RealLiteral &)       = 0;
  virtual E evaluate(ASTNode &, CharLiteral &)       = 0;
  virtual E evaluate(ASTNode &, StringLiteral &)     = 0;
  virtual E evaluate(ASTNode &, VectorLiteral &)     = 0;
  virtual E evaluate(ASTNode &, ArrayLiteral &)      = 0;
  virtual E evaluate(ASTNode &, DictionaryLiteral &) = 0;
  virtual E evaluate(ASTNode &, Assignment &)        = 0;
  virtual E evaluate(ASTNode &, Lambda &)            = 0;
  virtual E evaluate(ASTNode &, Call &)              = 0;

  virtual S execute(Empty &)       = 0;
  virtual S execute(Variable &)    = 0;
  virtual S execute(Package &)     = 0;
  virtual S execute(ExprStmt &)    = 0;
  virtual S execute(Block &)       = 0;
  virtual S execute(Conditional &) = 0;
  virtual S execute(Loop &)        = 0;
  virtual S execute(Foreach &)     = 0;
  virtual S execute(Match &)       = 0;
  virtual S execute(MatchCase &)   = 0;
  virtual S execute(ControlFlow &) = 0;
  virtual S execute(Return &)      = 0;
  virtual S execute(Function &)    = 0;
  virtual S execute(Constant &)    = 0;
  virtual S execute(Enum &)        = 0;
  virtual S execute(Object &)      = 0;
  virtual S execute(Main &)        = 0;

  /// TODoc
  E evaluate_expression(ASTNode &n) {
    return std::visit(
      [&](auto &&expr) { return this->evaluate(n, *expr); }, n.inner());
  }

  /// TODoc
  S execute_statement(Statement &s) {
    return std::visit([&](auto &&stmt) { return this->execute(*stmt); }, s);
  }

public:
  virtual ~ASTHandler() {
  }
};

/// TODoc
template <typename T, typename Y>
constexpr auto to(Y &&node) noexcept(false) -> T {
  return std::visit(
    [](auto &&node) -> T {
      using NodeT = typename std::decay_t<decltype(node)>::element_type;

      if constexpr (std::is_same_v<T, NodeT>) {
        return std::move(*node);
      } else {
        throw std::invalid_argument("statement is not of correct type");
      }
    },
    node);
}

/// TODoc
template <typename T, typename Y>
constexpr auto is(Y &&node) noexcept(false) -> bool {
  return std::visit(
    [](auto &&node) -> T {
      using NodeT = typename std::decay_t<decltype(node)>::element_type;

      if constexpr (std::is_same_v<T, NodeT>) {
        return true;
      } else {
        return false;
      }
    },
    node);
}

} // namespace silk
