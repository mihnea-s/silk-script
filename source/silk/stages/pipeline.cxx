#include <silk/stages/pipeline.h>

#include <silk/tools/cli.h>

namespace silk {

void Error::print(std::ostream &out) const noexcept {
  print_error(out, "{}", _message);
}

const char *Error::what() const noexcept {
  return _message.c_str();
}

void LocError::print(std::ostream &out) const noexcept {
  print_error(out, _location, "{}", _message);
}

} // namespace silk
