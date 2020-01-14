#include <string_view>

#include <fmt/core.h>
#include <fmt/format.h>

#include <silk/error.h>

template <>
struct fmt::formatter<Severity> : formatter<string_view> {
  template <typename FormatContext>
  auto format(Severity sev, FormatContext& ctx) {
    string_view name = "unknown";
    switch (sev) {
      case Severity::compiler: name = "compiler"; break;
      case Severity::error: name = "error"; break;
      case Severity::warning: name = "warning"; break;
      case Severity::info: name = "info"; break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};

// LexingError --------------

Severity LexingError::severity() const noexcept {
  return Severity::compiler;
}

const char* LexingError::what() const noexcept {
  return _message.data();
}

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