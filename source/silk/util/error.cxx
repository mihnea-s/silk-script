#include <silk/util/error.h>

namespace silk {

// ParsingError -------------

std::uint64_t SilkError::line() const noexcept {
  return _location.first;
}

std::uint64_t SilkError::column() const noexcept {
  return _location.second;
}

const char *SilkError::what() const noexcept {
  return _message.data();
}

auto ErrorReporter::has_error() const -> bool {
  return !_errors.empty();
}

auto ErrorReporter::clear_errors() -> void {
  _errors.clear();
}

auto ErrorReporter::errors() const -> const std::vector<SilkError> & {
  return _errors;
}

} // namespace silk
