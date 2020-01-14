#include <cstdint>

#include <silk/ast/token.h>

const TokenType& Token::type() const noexcept {
  return _type;
};

const std::string& Token::lexeme() const noexcept {
  return _lexeme;
}

const std::pair<std::uint64_t, std::uint64_t>& Token::location() const
  noexcept {
  return _location;
}
