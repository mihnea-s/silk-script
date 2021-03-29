#pragma once

#include <optional>
#include <string>

namespace silk {

using Location = std::pair<size_t, size_t>;

enum class TokenKind {
  IDENTIFIER, // <id>
  COMMENT,    // <cmnt>

  TOK_END,   // <end>
  TOK_ERROR, // <err>

  KW_MAIN,  // main
  KW_PKG,   // pkg
  KW_USE,   // use
  KW_FUN,   // fun
  KW_ENUM,  // enum
  KW_OBJ,   // obj
  KW_PRIV,  // priv
  KW_DLL,   // dll
  KW_MACRO, // macro

  KW_LET,      // let
  KW_DEF,      // def
  KW_CONST,    // const
  KW_RETURN,   // return
  KW_SWITCH,   // switch
  KW_BREAK,    // break
  KW_CONTINUE, // continue
  KW_IF,       // if
  KW_ELSE,     // else
  KW_WHILE,    // while
  KW_LOOP,     // loop
  KW_FOR,      // for
  KW_FOREACH,  // foreach
  KW_MATCH,    // match

  KW_AND, // and
  KW_OR,  // or
  KW_NOT, // not

  BOOL_TRUE,  // true
  BOOL_FALSE, // false

  KEY_VOID, // void
  KEY_PI,   // pi
  KEY_TAU,  // tau
  KEY_EUL,  // e

  LITERAL_NAT,    // <nat>
  LITERAL_INT,    // <int>
  LITERAL_REAL,   // <real>
  LITERAL_CHAR,   // <chr>
  LITERAL_STRING, // <str>

  SYM_COMMA,     // ,
  SYM_SEMICOLON, // ;
  SYM_RD_OPEN,   // (
  SYM_RD_CLOSE,  // )
  SYM_SQ_OPEN,   // [
  SYM_SQ_CLOSE,  // ]
  SYM_BR_OPEN,   // {
  SYM_BR_CLOSE,  // }

  SYM_PLUS,              // +
  SYM_PLUS_PLUS,         // ++    UNUSED
  SYM_PLUS_EQUAL,        // +=
  SYM_PLUS_PLUS_EQUAL,   // ++=   UNUSED
  SYM_MINUS,             // -
  SYM_ARROW,             // ->
  SYM_MINUS_MINUS,       // --    UNUSED
  SYM_MINUS_EQUAL,       // -=
  SYM_MINUS_MINUS_EQUAL, // --=   UNUSED
  SYM_SLASH,             // /
  SYM_SLASH_SLASH,       // //
  SYM_SLASH_EQUAL,       // /=
  SYM_SLASH_SLASH_EQUAL, // //=
  SYM_STAR,              // *
  SYM_STAR_STAR,         // **
  SYM_STAR_EQUAL,        // *=
  SYM_STAR_STAR_EQUAL,   // **=
  SYM_PERC,              // %
  SYM_PERC_PERC,         // %%    UNUSED
  SYM_PERC_EQUAL,        // %=
  SYM_PERC_PERC_EQUAL,   // %%=   UNUSED

  SYM_EQUAL,             // =
  SYM_EQUAL_EQUAL,       // ==
  SYM_FATARROW,          // =>
  SYM_EQUAL_EQUAL_EQUAL, // ===   UNUSED
  SYM_BANG,              // !     UNUSED
  SYM_BANG_BANG,         // !!    UNUSED
  SYM_BANG_EQUAL,        // !=
  SYM_BANG_BANG_EQUAL,   // !!=   UNUSED
  SYM_GT,                // >
  SYM_GT_GT,             // >>    UNUSED
  SYM_GT_GT_GT,          // >>>   UNUSED
  SYM_GT_EQUAL,          // >=
  SYM_GT_GT_EQUAL,       // >>=   UNUSED
  SYM_GT_GT_GT_EQUAL,    // >>>=  UNUSED
  SYM_LT,                // <
  SYM_LT_ARROW,          // <-
  SYM_LT_LT,             // <<    UNUSED
  SYM_LT_LT_LT,          // <<<   UNUSED
  SYM_LT_EQUAL,          // <=
  SYM_LT_LT_EQUAL,       // <<=   UNUSED
  SYM_LT_LT_LT_EQUAL,    // <<<=  UNUSED

  SYM_AMP,           // &
  SYM_AMP_AMP,       // &&  UNUSED
  SYM_PIPE,          // |   UNUSED
  SYM_PIPE_PIPE,     // ||  UNUSED
  SYM_TILDE,         // ~
  SYM_TILDE_TILDE,   // ~~  UNUSED
  SYM_CARET,         // ^   UNUSED
  SYM_CARET_CARET,   // ^^  UNUSED
  SYM_QMARK,         // ?   UNUSED
  SYM_QMARK_QMARK,   // ??  UNUSED
  SYM_ATSIGN,        // @   UNUSED
  SYM_ATSIGN_ATSIGN, // @@  UNUSED
  SYM_USCORE,        // _   UNUSED
  SYM_USCORE_USCORE, // __  UNUSED

  SYM_DOT,           // .
  SYM_DOT_DOT,       // ..  UNUSED
  SYM_DOT_DOT_DOT,   // ... UNUSED
  SYM_DOT_BRACE,     // .{  UNUSED
  SYM_COLON,         // :
  SYM_COLON_COLON,   // ::
  SYM_COLON_BRACE,   // :{  UNUSED
  SYM_DOLLAR,        // $   UNUSED
  SYM_DOLLAR_DOLLAR, // $$  UNUSED
  SYM_DOLLAR_BRACE,  // ${  UNUSED
  SYM_HASH_BRACE,    // #{
};

/// Token is a struct used for storing logical groupings
/// in the programs source code (e.g. `>=`), marking
/// keywords to help the parser (e.g. `fct`) and storing
/// literal values (e.g. `2.3`) (note: as raw strings),
/// the lexer turns the programs source file into an array
/// of tokens
struct Token {
  TokenKind   kind;     //< Which kind of token this is
  std::string lexeme;   //< Raw lexeme in the source file
  Location    location; //< Where this token occurs in the source file
};

} // namespace silk
