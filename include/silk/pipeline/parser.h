#pragma once

#include <silk/language/package.h>
#include <silk/language/scanner.h>
#include <silk/language/syntax_tree.h>
#include <silk/pipeline/stage.h>
#include <tuple>

namespace silk {

class Parser final : public NonSyntaxTreeStage<Parser, Source, Module> {
private:
  std::optional<Scanner> _scanner;
  std::vector<Token>     _tokens;

  enum class Precedence {
    ANY,        // lowest
    ASSIGNMENT, // = ..
    OR,         // or
    AND,        // and
    EQUALITY,   // == !=
    COMPARISON, // < > <= >=
    TERM,       // + -
    FACTOR,     // * / // %
    POWER,      // ** | ~
    UNARY,      // not -
    CALL,       // . ()
    NONE,       // highest
  };

  struct Rule {
    using UnaParseFN = std::unique_ptr<st::Node> (Parser::*)();
    using BinParseFN =
      std::unique_ptr<st::Node> (Parser::*)(std::unique_ptr<st::Node> &&);
    UnaParseFN prefix     = nullptr;
    BinParseFN infix      = nullptr;
    UnaParseFN postfix    = nullptr;
    Precedence precedence = Precedence::ANY;
  };

  static const std::unordered_map<TokenKind, Rule> rules;

  // check end of token stream
  inline auto eof() const -> bool;

  // move forward and return previous token
  inline auto advance() -> Token &;

  // peek the previous token
  inline auto previous() const -> const Token &;

  // peek the next token
  inline auto peek() const -> const Token &;

  // match functions return true if the next
  // token is equal to one of their arguments
  // false otherwise
  template <class... Args>
  inline auto match(Args... args) const -> bool {
    if (eof()) return false;

    for (auto kind : std::vector{args...}) {
      if (peek().kind == kind) return true;
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

  // Wrapper around match that adds an error if match returned false
  inline auto must_match(TokenKind, std::string_view) const -> void;

  /// Just like [`must_match`] but throws an error if the
  /// next token is equal to the argument
  inline auto must_consume(TokenKind, std::string_view) -> void;

  template <class T, class... Args>
  auto make_node(Args &&...args) -> std::unique_ptr<st::Node> {
    return std::unique_ptr<st::Node>(new st::Node{
      .location = peek().location,
      .data     = T{std::forward<Args>(args)...},
    });
  };

  // Pratt Parser functions
  auto precendece(Precedence) -> std::unique_ptr<st::Node>;
  auto higher(Precedence) const -> Precedence;
  auto lower(Precedence) const -> Precedence;
  auto get_rule(const Token &) const
    -> std::optional<std::reference_wrapper<const Rule>>;

  // Parsing helpers
  auto parse_identifier() -> std::string;
  auto parse_package() -> std::string;
  auto parse_typing() -> st::Typing;
  auto parse_typed_fields(TokenKind, TokenKind) -> st::TypedFields;

  // Function parsing helpers
  auto parse_named_function_header()
    -> std::tuple<std::string, st::TypedFields, st::Typing>;
  auto parse_nameless_function_header()
    -> std::tuple<st::TypedFields, st::Typing>;
  auto parse_function_body() -> std::unique_ptr<st::Node>;

  // Declarations & Module
  auto declaration() -> std::unique_ptr<st::Node>;
  auto declaration_main() -> std::unique_ptr<st::Node>;
  auto declaration_package() -> std::unique_ptr<st::Node>;
  auto declaration_import() -> std::unique_ptr<st::Node>;
  auto declaration_function() -> std::unique_ptr<st::Node>;
  auto declaration_enum() -> std::unique_ptr<st::Node>;
  auto declaration_object() -> std::unique_ptr<st::Node>;
  auto declaration_library() -> std::unique_ptr<st::Node>;
  auto declaration_macro() -> std::unique_ptr<st::Node>;

  // Statements
  auto statement() -> std::unique_ptr<st::Node>;
  auto statement_empty() -> std::unique_ptr<st::Node>;
  auto statement_expression() -> std::unique_ptr<st::Node>;
  auto statement_block() -> std::unique_ptr<st::Node>;
  auto statement_circuit() -> std::unique_ptr<st::Node>;
  auto statement_variable() -> std::unique_ptr<st::Node>;
  auto statement_constant() -> std::unique_ptr<st::Node>;
  auto statement_return() -> std::unique_ptr<st::Node>;
  auto statement_switch() -> std::unique_ptr<st::Node>;
  auto statement_itercontrol() -> std::unique_ptr<st::Node>;
  auto statement_if() -> std::unique_ptr<st::Node>;
  auto statement_while() -> std::unique_ptr<st::Node>;
  auto statement_loop() -> std::unique_ptr<st::Node>;
  auto statement_for() -> std::unique_ptr<st::Node>;
  auto statement_foreach() -> std::unique_ptr<st::Node>;
  auto statement_match() -> std::unique_ptr<st::Node>;

  // expressions
  auto expression() -> std::unique_ptr<st::Node>;
  auto expression_identifier() -> std::unique_ptr<st::Node>;
  auto expression_void() -> std::unique_ptr<st::Node>;
  auto expression_continuation() -> std::unique_ptr<st::Node>;
  auto expression_literal() -> std::unique_ptr<st::Node>;
  auto expression_char() -> std::unique_ptr<st::Node>;
  auto expression_string() -> std::unique_ptr<st::Node>;
  auto expression_unary() -> std::unique_ptr<st::Node>;
  auto expression_binary(std::unique_ptr<st::Node> &&)
    -> std::unique_ptr<st::Node>;
  auto expression_tuple() -> std::unique_ptr<st::Node>;
  auto expression_range(std::unique_ptr<st::Node> &&left)
    -> std::unique_ptr<st::Node>;
  auto expression_vector() -> std::unique_ptr<st::Node>;
  auto expression_array() -> std::unique_ptr<st::Node>;
  auto expression_dictionary() -> std::unique_ptr<st::Node>;
  auto expression_assignment(std::unique_ptr<st::Node> &&)
    -> std::unique_ptr<st::Node>;
  auto expression_call(std::unique_ptr<st::Node> &&)
    -> std::unique_ptr<st::Node>;
  auto expression_lambda() -> std::unique_ptr<st::Node>;

public:
  Parser() : _scanner(), _tokens() {
  }

  Parser(const Parser &) = delete;
  Parser(Parser &&)      = default;

  auto execute(Source &&source) noexcept -> Module override;
};

} // namespace silk
