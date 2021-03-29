#pragma once

#include <unordered_map>

#include <silk/language/token.h>

namespace silk {

class Scanner {
private:
  Location      _location;
  std::istream &_input;

  static const std::unordered_map<std::string_view, TokenKind> keywords;

  auto peek() const noexcept -> int;
  auto advance() noexcept -> int;
  auto compound(char, TokenKind, TokenKind) noexcept -> Token;

  auto scan_comment() noexcept -> Token;
  auto scan_char() noexcept -> Token;
  auto scan_string() noexcept -> Token;
  auto scan_number(int) noexcept -> Token;
  auto scan_identifier(int) -> std::string;

  auto make_token(TokenKind) const noexcept -> Token;
  auto make_token(TokenKind, std::string) const noexcept -> Token;

public:
  Scanner(std::istream &input) : _location({1, 0}), _input(input) {
  }

  auto scan() noexcept -> Token;
};

} // namespace silk