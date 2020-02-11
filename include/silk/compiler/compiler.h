#pragma once

#include <cstdint>
#include <map>
#include <vector>

#include <vm/chunk.h>
#include <vm/value.h>

#include "../common/error.h"
#include "../common/token.h"

class Compiler {
  private:
  using Iter = std::vector<Token>::const_iterator;

  Iter _tok;
  Iter _end;

  std::vector<ParsingError> _errors;
  std::vector<Chunk>        _chunks;

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
    using CompileFN = void (Compiler::*)(void);
    CompileFN  prefix;
    CompileFN  infix;
    CompileFN  postfix;
    Precedence prec;
  };

  static std::map<TokenType, Rule> rules;

  // position helper functions
  inline auto forward() -> void;
  inline auto backward() -> void;

  inline auto previous() -> Token;

  // move forward and return previous token
  inline auto advance() -> Token;

  // return current token
  inline auto current() const -> Token;

  // check end of token stream
  inline auto eof() const -> bool;

  // error helper
  inline auto throw_error(std::string) -> void;

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
  inline auto should_match(TokenType, std::string) const -> void;

  // wrapper around match that adds an error if match returned false
  inline auto must_match(TokenType, std::string) const -> void;

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
  inline auto should_consume(TokenType, std::string) -> void;
  inline auto must_consume(TokenType, std::string) -> void;

  // chunk functions
  inline auto current_chunk() -> Chunk*;
  inline auto cnst(Value) -> void;
  inline auto emit(std::uint8_t) -> void;

  // Pratt Parser functions
  auto get_rule(TokenType) -> Rule&;
  auto higher(Precedence) -> Precedence;
  auto lower(Precedence) -> Precedence;

  auto precendece(Precedence) -> void;

  // expressions
  auto expression() -> void;

  auto expr_unary() -> void;
  auto expr_binary() -> void;
  auto expr_grouping() -> void;

  // literals
  auto literal_integer() -> void;
  auto literal_double() -> void;
  auto literal_string() -> void;

  public:
  // error public methods
  auto has_error() const -> bool;
  auto clear_errors() -> void;
  auto errors() const -> const std::vector<ParsingError>&;

  // compile entrypoint
  auto compile(Iter begin, Iter end) noexcept -> std::vector<Chunk>;
};
