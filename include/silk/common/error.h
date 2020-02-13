#pragma once

#include <cstdint>
#include <exception>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include <fmt/core.h>
#include <fmt/format.h>

#include <util/message.h>

template <class... Args>
void print_error(std::string_view frmt, Args... args) {
  fmt::print(RED BOLD "(!) Error" RESET ": ");
  fmt::print(frmt, args...);
  fmt::print("\n");
}

template <class... Args>
void print_warning(std::string_view frmt, Args... args) {
  fmt::print(YELLOW BOLD "(*) Warning" RESET ": ");
  fmt::print(frmt, args...);
  fmt::print("\n");
}

struct SilkError : std::exception {
  public:
  enum Severity {
    ERROR,
    WARNING,
  };

  private:
  using Location = std::pair<std::uint64_t, std::uint64_t>;

  const Severity    _severity;
  const Location    _location;
  const std::string _message;

  public:
  SilkError(
    const Severity    severity,
    const std::string message,
    const Location    location) noexcept :
      _severity(severity), _location(location), _message(message) {
  }

  // getters
  Severity      severity() const noexcept;
  std::uint64_t line() const noexcept;
  std::uint64_t column() const noexcept;

  // std::exception requirement
  const char* what() const noexcept override;
};

// extend fmt::format to be able to output
// silk's error type

template <>
struct fmt::formatter<SilkError> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const SilkError& err, FormatContext& ctx) {
    std::ostringstream msg {};

    switch (err.severity()) {
      case SilkError::ERROR: {
        msg << BOLD RED "(!) Error" RESET ": parsing error\n";
        break;
      }
      case SilkError::WARNING: {
        msg << BOLD YELLOW "(*) Warning" RESET ": parsing error\n";
        break;
      }
    }

    msg << "\t" << BOLD "where" RESET ": line " BLUE << err.line()
        << RESET ", column " BLUE << err.column() << RESET "\n";

    msg << "\t" << BOLD "what" RESET ": " << err.what() << RESET "\n";

    return fmt::format_to(ctx.out(), "{}", msg.str());
  }
};

class ErrorReporter {
  protected:
  using Location = std::pair<std::uint64_t, std::uint64_t>;
  std::vector<SilkError> mutable _errors;

  template <class... Args>
  auto report(
    SilkError::Severity sev,
    Location            loc,
    std::string_view    frmt,
    Args... args) const {
    _errors.emplace_back(
      sev, fmt::format(frmt, std::forward<Args>(args)...), loc);
    return _errors.back();
  }

  template <class... Args>
  constexpr auto
  report_warning(Location loc, std::string_view frmt, Args... args) const {
    return report(SilkError::WARNING, loc, frmt, std::forward<Args>(args)...);
  }

  template <class... Args>
  constexpr auto
  report_error(Location loc, std::string_view frmt, Args... args) const {
    return report(SilkError::ERROR, loc, frmt, std::forward<Args>(args)...);
  }

  public:
  // error public methods
  auto has_warning() const -> bool;
  auto has_error() const -> bool;
  auto clear_errors() -> void;
  auto errors() const -> const std::vector<SilkError>&;
};