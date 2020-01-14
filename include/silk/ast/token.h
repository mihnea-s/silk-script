#pragma once

#include <cstdint>
#include <string>

enum class TokenType {
  // symbols ----------------

  // maths operations
  sym_plus,
  sym_plusplus,
  sym_plusequal,
  sym_minus,
  sym_minusminus,
  sym_minusequal,
  sym_slash,
  sym_slashslash,
  sym_star,
  sym_starstar,
  sym_percent,

  // assignment
  sym_equal,

  // comparisons
  sym_equalequal,
  sym_bangequal,
  sym_gt,
  sym_gtequal,
  sym_lt,
  sym_ltequal,

  // bitwise ops
  sym_amp,
  sym_pipe,
  sym_tilde,
  sym_caret,

  // boolean operations
  sym_ampamp,
  sym_pipepipe,
  sym_bang,

  // other
  sym_colon,
  sym_semicolon,
  sym_question,
  sym_dot,
  sym_comma,
  sym_at,

  // parenthesis
  sym_lround,
  sym_rround,
  sym_lsquare,
  sym_rsquare,
  sym_lbrace,
  sym_rbrace,

  // keywords ---------------

  kw_function,
  kw_struct,
  kw_ctor,
  kw_dtor,
  kw_virt,
  kw_super,
  kw_this,

  kw_package,
  kw_entrypoint,
  kw_import,
  kw_export,
  kw_forwarding,

  kw_true,
  kw_false,
  kw_vid,

  kw_let,
  kw_if,
  kw_else,
  kw_for,
  kw_break,
  kw_continue,
  kw_return,

  // literals
  literal_int,
  literal_dbl,
  literal_str,

  // other
  identifier,
  eof,
};

struct Token {
  const TokenType                               _type;
  const std::string                             _lexeme;
  const std::pair<std::uint64_t, std::uint64_t> _location;

  public:
  Token(
    TokenType                                     type,
    const std::pair<std::uint64_t, std::uint64_t> location) :
      _type(type), _lexeme(std::string {}), _location(location) {
  }

  Token(
    TokenType                                      type,
    const std::string&                             value,
    const std::pair<std::uint64_t, std::uint64_t>& location) :
      _type(type), _lexeme(value), _location(location) {
  }

  const TokenType&                               type() const noexcept;
  const std::string&                             lexeme() const noexcept;
  const std::pair<std::uint64_t, std::uint64_t>& location() const noexcept;
};
