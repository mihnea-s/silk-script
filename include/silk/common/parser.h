#pragma once

#include <cstdint>
#include <map>
#include <utility>
#include <vector>

#include "ast.h"
#include "error.h"
#include "token.h"

class Parser : public ErrorReporter {
  private:
  using Iter = std::vector<Token>::const_iterator;

  Iter _tok;
  Iter _end;

  enum class Precedence {
    ANY,        // lowest
    ASSIGNMENT, // =
    OR,         // ||
    AND,        // &&
    EQUALITY,   // == !=
    COMPARISON, // < > <= >=
    TERM,       // + -
    FACTOR,     // * / // %
    POWER,      // **
    UNARY,      // ! -
    CALL,       // . ()
    NONE,       // highest
  };

  struct Rule {
    using UnParseFN    = ASTNodePtr (Parser::*)();
    using BinParseFN   = ASTNodePtr (Parser::*)(ASTNodePtr);
    UnParseFN  prefix  = nullptr;
    BinParseFN infix   = nullptr;
    UnParseFN  postfix = nullptr;
    Precedence prec    = Precedence::ANY;
  };

  static std::unordered_map<TokenType, Rule> rules;

  // position helper functions
  inline auto forward() -> void;
  inline auto backward() -> void;

  // move forward and return previous token
  inline auto advance() -> Token;

  // token access
  inline auto previous() const -> Token;
  inline auto current() const -> Token;

  // check end of token stream
  inline auto eof() const -> bool;

  // error helper
  inline auto error_location() const -> std::pair<std::uint64_t, std::uint64_t>;

  // match functions return true if the next
  // token is equal to one of their arguments
  // false otherwise
  template <class... Args>
  inline auto match(Args... args) const -> bool {
    if (eof()) return false;

    for (auto type : std::vector {args...}) {
      if ((*_tok).type() == type) return true;
    }

    return false;
  };

  // wrapper around match that adds a warning if match returned false
  inline void should_match(TokenType, std::string) const;

  // wrapper around match that adds an error if match returned false
  inline void must_match(TokenType, std::string) const;

  // consume functions return true *and move forward* if
  // the current token is equal to one of their arguments
  // they return false otherwise
  template <class... Args>
  inline auto consume(Args... args) -> bool {
    if (match(args...)) {
      advance();
      return true;
    }

    return false;
  }

  // see should_match & must_match
  inline void should_consume(TokenType, std::string);
  inline void must_consume(TokenType, std::string);

  template <class T, class... Args>
  auto make_node(Args... args) -> ASTNodePtr {
    return std::unique_ptr<ASTNode>(static_cast<ASTNode*>(new T {
      previous().location(),
      T::node_enum,
      std::forward<Args>(args)...,
    }));
  };

  // Pratt Parser functions
  auto get_rule(TokenType) -> Rule&;
  auto higher(Precedence) -> Precedence;
  auto lower(Precedence) -> Precedence;

  auto precendece(Precedence) -> ASTNodePtr;

  // expressions
  auto expression() -> ASTNodePtr;

  auto expr_unary() -> ASTNodePtr;
  auto expr_binary(ASTNodePtr) -> ASTNodePtr;
  auto expr_grouping() -> ASTNodePtr;

  // literals
  auto literal_integer() -> ASTNodePtr;
  auto literal_real() -> ASTNodePtr;
  auto literal_string() -> ASTNodePtr;

  // constant values
  auto constant_value() -> ASTNodePtr;

  public:
  // parsing function
  auto parse(Iter begin, Iter end) noexcept -> AST;
};
