#pragma once

#include <istream>
#include <optional>
#include <string>

#include <silk/util/error.h>

/// TODO
enum class TokenKind {
  IDENTIFIER,

  TOK_END,
  TOK_ERROR,

  SYM_PLUS,
  SYM_PLUSPLUS, // UNUSED
  SYM_PLUSEQUAL,
  SYM_MINUS,
  SYM_MINUSMINUS, // UNUSED
  SYM_MINUSEQUAL,
  SYM_SLASH,
  SYM_SLASHSLASH,
  SYM_STAR,
  SYM_STARSTAR,
  SYM_PERCENT,
  SYM_PERCPERC, // UNUSED

  SYM_EQUAL,
  SYM_EQUALEQUAL,
  SYM_BANGEQUAL,
  SYM_GT,
  SYM_GTEQUAL,
  SYM_LT,
  SYM_LTEQUAL,

  SYM_ARROW,    // UNUSED
  SYM_RARROW,   // UNUSED
  SYM_FATARROW, // UNUSED

  SYM_AMP,
  SYM_AMPAMP, // UNUSED
  SYM_PIPE,
  SYM_PIPEPIPE, // UNUSED
  SYM_TILDE,
  SYM_BANG, // UNUSED
  SYM_CARET,

  SYM_COLON,
  SYM_COLONCOLON,
  SYM_SEMICOLON,
  SYM_QMARK,      // UNUSED
  SYM_QMARKQMARK, // UNUSED
  SYM_DOT,
  SYM_DOTDOT, // UNUSED
  SYM_COMMA,
  SYM_AT,   // UNUSED
  SYM_ATAT, // UNUSED
  SYM_USCORE,
  SYM_USCOREUSCORE, // UNUSED

  SYM_LROUND,
  SYM_RROUND,
  SYM_LSQUARE,
  SYM_RSQUARE,
  SYM_LBRACE,
  SYM_RBRACE,

  KW_PKG,
  KW_MAIN,
  KW_USE,
  KW_EXTERN,

  KW_LET,
  KW_DEF,
  KW_CONST,
  KW_FUN,
  KW_ENUM,
  KW_STRUCT,

  KW_IF,
  KW_ELSE,
  KW_FOR,
  KW_MATCH,

  KW_RETURN,
  KW_BREAK,
  KW_CONTINUE,

  KW_AND,
  KW_OR,
  KW_NOT,

  BOOL_TRUE,
  BOOL_FALSE,

  KEY_VOID,
  KEY_PI,
  KEY_TAU,
  KEY_EUL,

  LITERAL_INTEGER,
  LITERAL_DOUBLE,
  LITERAL_CHAR,
  LITERAL_STRING,
};

/// Token is a struct used for storing logical groupings
/// in the programs source code (e.g. `>=`), marking
/// keywords to help the parser (e.g. `fct`) and storing
/// literal values (e.g. `2.3`) (note: as raw strings),
/// the lexer turns the programs source file into an array
/// of tokens
struct Token {
  TokenKind   kind;     //< Which kind of token this is
  std::string lexeme;   //< Raw lexeme for literals and identifiers
  Location    location; //< Where this token occurs in the source file
};

/// TODO
struct TokenScanner {
private:
  Location      _location;
  std::istream &_input;

  static const std::unordered_map<std::string_view, TokenKind> keywords;

  auto peek() const noexcept -> int;
  auto advance() noexcept -> int;
  auto compound(char, TokenKind, TokenKind) noexcept -> Token;

  auto scan_char() noexcept -> Token;
  auto scan_string() noexcept -> Token;
  auto scan_number(int) noexcept -> Token;
  auto scan_identifier(int) -> std::string;

  auto make_token(TokenKind) const noexcept -> Token;
  auto make_token(TokenKind, std::string) const noexcept -> Token;

public:
  TokenScanner(std::istream &input) : _location({1, 0}), _input(input) {
  }

  auto scan() noexcept -> Token;
};
