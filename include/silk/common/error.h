#pragma once

#include <cstdint>
#include <exception>
#include <sstream>
#include <string>
#include <string_view>

#include <fmt/core.h>
#include <fmt/format.h>

#include <util/message.h>

// different severity levels
enum class Severity {
  error,
  warning,
};

// print errors

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

// ParsingErrors are used by the parser and type checker

struct ParsingError : std::exception {
  private:
  // for brevity
  using Location = std::pair<std::uint64_t, std::uint64_t>;

  const Severity    _severity;
  const Location    _location;
  const std::string _message;

  public:
  ParsingError(
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

// CompileErrors are used for errors (or warnings) that occur during the
// compilation of a silk script

struct CompileError : std::exception {
  private:
  // for brevity
  using Location = std::pair<std::uint64_t, std::uint64_t>;

  const Severity    _severity;
  const Location    _location;
  const std::string _message;

  public:
  CompileError(
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

// RuntimeError is used for errors that occur during the interpretation of a
// silk script

struct RuntimeError : std::exception {
  private:
  std::string _message;

  public:
  RuntimeError(std::string message) : _message(message) {
  }

  // std::exception requirement
  const char* what() const noexcept override;
};

// extend fmt::format to be able to output
// silk's error types

template <>
struct fmt::formatter<ParsingError> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ParsingError& err, FormatContext& ctx) {
    std::ostringstream msg {};

    switch (err.severity()) {
      case Severity::error: {
        msg << BOLD RED "(!) Error" RESET ": parsing error\n";
        break;
      }
      case Severity::warning: {
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

template <>
struct fmt::formatter<RuntimeError> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const RuntimeError& err, FormatContext& ctx) {
    std::ostringstream msg {};

    msg << BOLD RED "(!) Error" RESET ": runtime error\n";
    msg << "\t" << BOLD "what" RESET ": " << err.what() << RESET "\n";

    return fmt::format_to(ctx.out(), "{}", msg.str());
  }
};