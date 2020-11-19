#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include <silk/lexer/token.h>
#include <silk/parser/ast.h>
#include <silk/util/error.h>

class Parser : public ErrorReporter {
  private:
  using Iter = std::vector<Token>::const_iterator;

  Iter _tok;
  Iter _end;

  enum class Precedence {
    ANY,        // lowest
    ASSIGNMENT, // =
    OR,         // || is isnt
    AND,        // &&
    EQUALITY,   // == !=
    COMPARISON, // < > <= >=
    TERM,       // + -
    FACTOR,     // * / // %
    POWER,      // **
    UNARY,      // ! - typeof
    CALL,       // . ()
    NONE,       // highest
  };

  struct Rule {
    using UnParseFN    = ASTNode (Parser::*)();
    using BinParseFN   = ASTNode (Parser::*)(ASTNode&&);
    UnParseFN  prefix  = nullptr;
    BinParseFN infix   = nullptr;
    UnParseFN  postfix = nullptr;
    Precedence prec    = Precedence::ANY;
  };

  static const Rule no_rule;

  static const std::unordered_map<TokenType, Rule> rules;

  // move forward and return previous token
  inline auto advance() -> const Token&;

  // token access
  inline auto previous() const -> const Token&;
  inline auto current() const -> const Token&;

  // check end of token stream
  inline auto eof() const -> bool;

  // error helper
  inline auto error_location() const -> std::pair<std::uint64_t, std::uint64_t>;

  // match functions return true if the next
  // token is equal to one of their arguments
  // false otherwise
  template <class... Args>
  inline auto can_match(Args... args) const -> bool {
    if (eof()) return false;

    for (auto type : std::vector {args...}) {
      if ((*_tok).type == type) return true;
    }

    return false;
  };

  // consume functions return true *and move forward* if
  // the current token is equal to one of their arguments
  // they return false otherwise
  template <class... Args>
  inline auto can_consume(Args... args) -> bool {
    if (match(args...)) {
      advance();
      return true;
    }

    return false;
  }

  // wrapper around match that adds an error if match returned false
  inline auto must_match(TokenType, std::string) const -> void;

  // see should_match & must_match
  inline auto must_consume(TokenType, std::string) -> void;

  template <class T, class... Args>
  auto make_node(Args... args) -> ASTNode {
    return ASTNode {
      .location   = previous().location,
      .typing     = nullptr,
      .expression = std::make_unique(std::forward<Args>(args)...),
    };
  };

  // Pratt Parser functions
  auto get_rule(const Token&) const -> const Rule&;
  auto higher(Precedence) const -> Precedence;
  auto lower(Precedence) const -> Precedence;

  auto precendece(Precedence) -> ASTNode;

  auto next_assign() -> bool;

  auto parse_name() -> std::string;
  auto parse_package() -> std::string;
  auto parse_typing() -> Typing;
  auto parse_typed_fields() -> TypedFields;

  auto declaration() -> Statement;
  auto statement() -> Statement;
  auto expression() -> ASTNode;

  // declarations
  auto decl_package() -> Statement;
  auto decl_variable() -> Statement;
  auto decl_function() -> Statement;
  auto decl_enum() -> Statement;
  auto decl_struct() -> Statement;

  // statements
  auto stmt_empty() -> Statement;
  auto stmt_exprstmt() -> Statement;
  auto stmt_block() -> Statement;
  auto stmt_conditional() -> Statement;
  auto stmt_loop() -> Statement;
  auto stmt_match() -> Statement;
  auto stmt_match_case() -> Statement;
  auto stmt_control_flow() -> Statement;
  auto stmt_return() -> Statement;

  // expressions
  auto expr_identifier() -> ASTNode;
  auto expr_unary() -> ASTNode;
  auto expr_binary(ASTNode &&) -> ASTNode;
  auto expr_const() -> ASTNode;
  auto expr_literal() -> ASTNode;
  auto expr_lambda() -> ASTNode;
  auto expr_assignment(ASTNode &&) -> ASTNode;
  auto expr_call(ASTNode &&) -> ASTNode;
  auto expr_grouping() -> ASTNode;
  auto expr_array() -> ASTNode;
  auto expr_constexpr() -> ASTNode;

  public:
  // parsing function
  auto parse(Iter begin, Iter end) noexcept -> AST;
};
