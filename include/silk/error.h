#pragma once

#include <cstdint>
#include <exception>
#include <string>
#include <string_view>
#include <tuple>

// different severity levels
enum class Severity {
  error,
  warning,
};

// ParsingErrors are used by the parser and type checker

struct ParsingError : std::exception {
  private:
  // for brevity
  using Location = std::pair<std::uint64_t, std::uint64_t>;

  const Severity         _severity;
  const Location         _location;
  const std::string_view _message;

  public:
  ParsingError(
    const Severity         severity,
    const std::string_view message,
    const Location         location) noexcept :
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

// pretty print functions
auto print_error(const ParsingError& e) -> void;
auto print_error(const RuntimeError& e) -> void;