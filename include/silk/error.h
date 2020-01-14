#pragma once

#include <cstdint>
#include <exception>
#include <string>
#include <string_view>
#include <tuple>

enum class Severity {
  error,
  warning,
  info,
  compiler,
};

struct LexingError : std::exception {
  private:
  const std::string_view _message;

  public:
  LexingError(std::string_view message) : _message(message) {
  }

  Severity    severity() const noexcept;
  const char* what() const noexcept override;
};

struct ParsingError : std::exception {
  private:
  const Severity                                _severity;
  const std::pair<std::uint64_t, std::uint64_t> _location;
  const std::string_view                        _message;

  public:
  ParsingError(
    const Severity                                severity,
    const std::string_view                        message,
    const std::pair<std::uint64_t, std::uint64_t> location) noexcept :
      _severity(severity), _location(location), _message(message) {
  }

  Severity      severity() const noexcept;
  std::uint64_t line() const noexcept;
  std::uint64_t column() const noexcept;
  const char*   what() const noexcept override;
};

struct RuntimeError : std::exception {
  private:
  std::string _message;

  public:
  RuntimeError(std::string message) : _message(message) {
  }

  const char* what() const noexcept override;
};