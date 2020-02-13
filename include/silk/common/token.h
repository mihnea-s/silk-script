#pragma once

#include <cstdint>
#include <string>

enum class TokenType {
  // symbols ----------------

  sym_plus,       // +
  sym_plusplus,   // ++
  sym_plusequal,  // +=
  sym_minus,      // -
  sym_minusminus, // --
  sym_minusequal, // -=
  sym_slash,      // /
  sym_slashslash, // //
  sym_star,       // *
  sym_starstar,   // **
  sym_percent,    // %

  sym_equal,      // =
  sym_equalequal, // ==
  sym_bangequal,  // !=
  sym_gt,         // >
  sym_gtequal,    // >=
  sym_lt,         // <
  sym_ltequal,    // <=

  sym_amp,      // &
  sym_ampamp,   // &&
  sym_pipe,     // |
  sym_pipepipe, // ||
  sym_tilde,    // ~
  sym_bang,     // !
  sym_caret,    // ^

  sym_colon,     // :
  sym_semicolon, // ;
  sym_question,  // ?
  sym_dot,       // .
  sym_comma,     // ,
  sym_at,        // @

  sym_lround,  // (
  sym_rround,  // )
  sym_lsquare, // [
  sym_rsquare, // ]
  sym_lbrace,  // {
  sym_rbrace,  // }

  // keywords ---------------

  kw_function, // fct
  kw_struct,   // struct
  kw_ctor,     // ctor
  kw_dtor,     // dtor
  kw_virt,     // virt

  kw_package, // pkg
  kw_main,    // main
  kw_import,  // use

  kw_let,      // let
  kw_if,       // if
  kw_else,     // else
  kw_for,      // for
  kw_break,    // break
  kw_continue, // continue
  kw_return,   // return

  // literals and words -----
  literal_int,
  literal_dbl,
  literal_str,
  identifier,

  // constants --------------
  cnst_true,  // true
  cnst_false, // false
  cnst_vid,   // vid

  cnst_pi,  // pi
  cnst_tau, // tau
  cnst_eul, // euler
};

// Token is a struct used for storing logical groupings
// in the programs source code (e.g. `>=`), marking
// keywords to help the parser (e.g. `fct`) and storing
// literal values (e.g. `2.3`) (note: as raw strings),
// the lexer turns the programs source file into an array
// of tokens

struct Token {
  private:
  // for brevity
  using Location = std::pair<std::uint64_t, std::uint64_t>;

  const TokenType   _type;     // type
  const std::string _lexeme;   // for literals and identifiers
  const Location    _location; // in file

  public:
  Token(TokenType type, const std::string& lexeme, Location location) :
      _type(type), _lexeme(lexeme), _location(location) {
  }

  // getters
  const TokenType&   type() const noexcept;
  const std::string& lexeme() const noexcept;
  const Location&    location() const noexcept;
};
