#pragma once

#include <cstdint>
#include <functional>
#include <istream>
#include <map>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include "ast/token.h"
#include "error.h"

// TODO
// - raw string literals
// - foreign code blocks

class Lexer {
  using TokensVector = std::vector<Token>;

  static const std::map<std::string_view, TokenType> _keywords;

  TokensVector _tokens;

  int line   = 0;
  int column = 0;

  std::string current_line = std::string {};

  inline void advance_line();
  inline void advance_column();

  inline bool next(char c) const;
  inline char peek() const;
  inline bool eol() const;
  inline bool alphanum(char c) const;

  template <class... Args>
  inline void add(Args... args) {
    _tokens.push_back(Token {args..., {line, column}});
  };

  inline void string();
  inline void number(bool real = false);

  inline std::string_view word();

  void scan_tokens(std::istream& in);

  public:
  const TokensVector& scan(std::istream& in) noexcept;

  std::function<void(LexingError&)> _error_callback;
  auto set_error_callback(std::function<void(LexingError&)> cb) -> void {
    _error_callback = cb;
  }
};