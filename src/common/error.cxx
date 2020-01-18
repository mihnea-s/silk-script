#include <silk/common/error.h>

// ParsingError -------------

Severity ParsingError::severity() const noexcept {
  return _severity;
}

std::uint64_t ParsingError::line() const noexcept {
  return _location.first;
}

std::uint64_t ParsingError::column() const noexcept {
  return _location.second;
}

const char* ParsingError::what() const noexcept {
  return _message.data();
}

// RuntimeError -------------

const char* RuntimeError::what() const noexcept {
  return _message.data();
}

// --------------------------