#include <silk/common/error.h>

// ParsingError -------------

SilkError::Severity SilkError::severity() const noexcept {
  return _severity;
}

std::uint64_t SilkError::line() const noexcept {
  return _location.first;
}

std::uint64_t SilkError::column() const noexcept {
  return _location.second;
}

const char* SilkError::what() const noexcept {
  return _message.data();
}

auto ErrorReporter::has_warning() const -> bool {
  return !_errors.empty();
}

auto ErrorReporter::has_error() const -> bool {
  for (const auto& error : _errors) {
    if (error.severity() == SilkError::ERROR) return true;
  }

  return false;
}

auto ErrorReporter::clear_errors() -> void {
  _errors.clear();
}

auto ErrorReporter::errors() const -> const std::vector<SilkError>& {
  return _errors;
}
