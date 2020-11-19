#pragma once

#include <initializer_list>
#include <map>
#include <string_view>
#include <vector>

#include "error.h"
#include "token.h"

// the lexer is tasked with splitting the program source code into easily
// digestible logical tokens and settling ambiguity between operators (e.g.
// 'x+++;')

class Lexer {
  std::vector<Token> _tokens;

  // location is source file
  int line   = 0;
  int column = 0;

  std::string current_line = std::string {};

  // used for indication only
  // it does not update current_line
  inline void advance_line();

  // advance to the next character in the line
  inline void advance_column();

  // returns true if the next character in the line is equal to c
  inline bool next(char c) const;

  // returns the current character in the line
  inline char peek() const;

  // returns true if we are at the end of the current line
  inline bool eol() const;

  // wrapper around std::isalnum to allow underscores
  inline bool alphanum(char c) const;

  // checks for compound tokens
  inline void compound(std::string_view, std::initializer_list<TokenType>);

  inline TokenType matchesRest(std::string_view, std::string_view, TokenType);

  inline TokenType keyword(std::string_view);

  // helper method to easily add tokens
  template <class... Args>
  inline void add(Args... args) {
    _tokens.push_back(Token {args..., {line, column}});
  };

  // add a string literal token to the output
  inline void string();

  // add a number literal token to the output
  inline void number(bool real = false);

  // get the current word in the line
  // (e.g. '[h]ello world' would return 'hello'
  // and the position would advance to 'hello [w]orld')
  inline std::string_view word();

  public:
  // the lexer's public interface
  const std::vector<Token>& scan(std::istream& in) noexcept;
};