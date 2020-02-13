#include <cstdint>

#include <silk/common/token.h>

// Token getters

const TokenType& Token::type() const noexcept {
  return _type;
};

const std::string& Token::lexeme() const noexcept {
  return _lexeme;
}

const Token::Location& Token::location() const noexcept {
  return _location;
}
