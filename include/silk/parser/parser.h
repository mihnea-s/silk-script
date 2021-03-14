#pragma once

#include <cstdint>
#include <istream>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include <silk/parser/ast.h>
#include <silk/parser/token.h>
#include <silk/util/error.h>

namespace silk {

/// TODoc
class Parser : public ErrorReporter {
private:
  TokenScanner _scanner;

  Token _prev;
  Token _next;

  enum class Precedence {
    ANY,        // lowest
    ASSIGNMENT, // =
    OR,         // or
    AND,        // and
    EQUALITY,   // == !=
    COMPARISON, // < > <= >=
    TERM,       // + -
    FACTOR,     // * / // %
    POWER,      // ** |
    UNARY,      // not -
    CALL,       // . ()
    NONE,       // highest
  };

  struct Rule {
    using UnParseFN    = ASTNode (Parser::*)();
    using BinParseFN   = ASTNode (Parser::*)(ASTNode &&);
    UnParseFN  prefix  = nullptr;
    BinParseFN infix   = nullptr;
    UnParseFN  postfix = nullptr;
    Precedence prec    = Precedence::ANY;
  };

  static const std::unordered_map<TokenKind, Rule> rules;

  // check end of token stream
  inline auto eof() const -> bool;

  // move forward and return previous token
  inline auto advance() -> Token &;

  // match functions return true if the next
  // token is equal to one of their arguments
  // false otherwise
  template <class... Args>
  inline auto match(Args... args) const -> bool {
    if (eof()) return false;

    for (auto kind : std::vector{args...}) {
      if (_next.kind == kind) return true;
    }

    return false;
  };

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

  // wrapper around match that adds an error if match returned false
  inline auto must_match(TokenKind, std::string) const -> void;

  /// Just like [`must_match`] but throws an error if the
  /// next token is equal to the argument
  inline auto must_consume(TokenKind, std::string) -> void;

  template <class T, class... Args>
  auto make_node(Args... args) -> ASTNode {
    return ASTNode(
      _prev.location,
      nullptr,
      std::move(std::make_unique<T>(std::forward<Args>(args)...)));
  };

  // Pratt Parser functions
  auto precendece(Precedence) -> ASTNode;
  auto higher(Precedence) const -> Precedence;
  auto lower(Precedence) const -> Precedence;
  auto get_rule(const Token &) const
    -> std::optional<std::reference_wrapper<const Rule>>;

  auto parse_name() -> std::string;
  auto parse_package() -> std::string;
  auto parse_typing() -> Typing;
  auto parse_typed_fields(TokenKind, TokenKind) -> TypedFields;

  auto declaration() -> Statement;
  auto statement() -> Statement;
  auto expression() -> ASTNode;

  // declarations
  auto decl_package() -> Statement;
  auto decl_constant() -> Statement;
  auto decl_function() -> Statement;
  auto decl_enum() -> Statement;
  auto decl_object() -> Statement;
  auto decl_main() -> Statement;

  // statements
  auto stmt_empty() -> Statement;
  auto stmt_variable() -> Statement;
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
  auto expr_literal() -> ASTNode;
  auto expr_vector() -> ASTNode;
  auto expr_array() -> ASTNode;
  auto expr_dictionary() -> ASTNode;
  auto expr_assignment(ASTNode &&) -> ASTNode;
  auto expr_lambda() -> ASTNode;
  auto expr_call(ASTNode &&) -> ASTNode;
  auto expr_grouping() -> ASTNode;

public:
  /// TODoc
  Parser(std::istream &source) : _scanner({source}) {
    // Initialize tokens
    this->_prev.kind = TokenKind::TOK_ERROR;
    this->_next      = _scanner.scan();
  }

  /// TODoc
  /// Parse the full source file as a list of declarations
  auto parse_source() noexcept -> AST;

  /// TODoc
  /// Parse a single declaration or a single expression
  auto parse_line() noexcept -> AST;
};

} // namespace silk
