#pragma once

#include <cstdint>
#include <string>

#include <silk/util/error.h>

enum class TokenType {
  IDENTIFIER,

  // symbols ----------------

  SYM_PLUS,       // +
  SYM_PLUSPLUS,   // ++ (UNUSED)
  SYM_PLUSEQUAL,  // +=
  SYM_MINUS,      // -
  SYM_MINUSMINUS, // -- (UNUSED)
  SYM_MINUSEQUAL, // -=
  SYM_SLASH,      // /
  SYM_SLASHSLASH, // //
  SYM_STAR,       // *
  SYM_STARSTAR,   // **
  SYM_PERCENT,    // %
  SYM_PERCPERC,   // %% (UNUSED)

  SYM_EQUAL,      // =
  SYM_EQUALEQUAL, // ==
  SYM_BANGEQUAL,  // !=
  SYM_GT,         // >
  SYM_GTEQUAL,    // >=
  SYM_LT,         // <
  SYM_LTEQUAL,    // <=

  SYM_ARROW,  // -> (UNUSED)
  SYM_RARROW, // <- (UNUSED)

  SYM_AMP,      // &
  SYM_AMPAMP,   // && (UNUSED)
  SYM_PIPE,     // |
  SYM_PIPEPIPE, // || (UNUSED)
  SYM_TILDE,    // ~
  SYM_BANG,     // !  (UNUSED)
  SYM_CARET,    // ^

  SYM_COLON,        // :
  SYM_COLONCOLON,   // ::
  SYM_SEMICOLON,    // ;
  SYM_QMARK,        // ?  (UNUSED)
  SYM_QMARKQMARK,   // ?? (UNUSED)
  SYM_DOT,          // .
  SYM_DOTDOT,       // .. (UNUSED)
  SYM_COMMA,        // ,
  SYM_AT,           // @  (UNUSED)
  SYM_ATAT,         // @@ (UNUSED)
  SYM_USCORE,       // _
  SYM_USCOREUSCORE, // __ (UNUSED)

  SYM_LROUND,  // (
  SYM_RROUND,  // )
  SYM_LSQUARE, // [
  SYM_RSQUARE, // ]
  SYM_LBRACE,  // {
  SYM_RBRACE,  // }

  // keywords ---------------

  KW_PKG,    // pkg
  KW_MAIN,   // main
  KW_USE,    // use
  KW_EXTERN, // extern

  KW_LET,    // let
  KW_DEF,    // def
  KW_CONST,  // const
  KW_FUN,    // fun
  KW_ENUM,   // enum
  KW_STRUCT, // struct

  KW_IF,    // if
  KW_ELSE,  // else
  KW_FOR,   // for
  KW_MATCH, // match

  KW_RETURN,   // return
  KW_BREAK,    // break
  KW_CONTINUE, // continue

  KW_AND, // and
  KW_OR,  // or
  KW_NOT, // not

  // literals and words -----

  LITERAL_INTEGER,
  LITERAL_DOUBLE,
  LITERAL_CHAR,
  LITERAL_STRING,

  // constants --------------

  CNST_TRUE,  // true
  CNST_FALSE, // false
  CNST_VOID,  // void
  CNST_PI,    // pi
  CNST_TAU,   // tau
  CNST_EUL,   // euler
};

/** Token is a struct used for storing logical groupings
 * in the programs source code (e.g. `>=`), marking
 * keywords to help the parser (e.g. `fct`) and storing
 * literal values (e.g. `2.3`) (note: as raw strings),
 * the lexer turns the programs source file into an array
 * of tokens
 */
struct Token {
  TokenType   type;     // type
  std::string lexeme;   // for literals and identifiers
  Location    location; // in file
};
