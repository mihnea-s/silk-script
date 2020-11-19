#pragma once

#include <sstream>

#include <fmt/format.h>

#include <silk/util/cli.h>
#include <utility>

using Location = std::pair<std::uint64_t, std::uint64_t>;

struct SilkError : std::exception {
  private:
  const Location    _location;
  const std::string _message;

  public:
  SilkError(const std::string message, const Location location) noexcept :
      _location(std::move(location)), _message(std::move(message)) {
  }

  // getters
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

    msg << BOLD RED "(!) Error" RESET ":" << std::endl;

    msg << "\t" << BOLD "where" RESET ": line " BLUE << err.line()
        << RESET ", column " BLUE << err.column() << RESET << std::endl;

    msg << "\t" << BOLD "what" RESET ": " << err.what() << RESET << std::endl;

    return fmt::format_to(ctx.out(), "{}", msg.str());
  }
};

class ErrorReporter {
  protected:
  std::vector<SilkError> mutable _errors;

  template <class... Args>
  auto report(Location loc, std::string_view frmt, Args... args) const {
    _errors.emplace_back(fmt::format(frmt, std::forward<Args>(args)...), loc);
    return _errors.back();
  }

  public:
  // error public methods
  auto has_error() const -> bool;
  auto clear_errors() -> void;
  auto errors() const -> const std::vector<SilkError>&;
};

template <class... Args>
void print_error(std::ostream& out, std::string_view frmt, Args... args) {
  out << BOLD RED "(!) Error" RESET ":" << std::endl;
  out << "\t" << BOLD "what" RESET ": "
      << fmt::format(frmt, std::forward<Args>(args)...) << RESET << std::endl;
}

template <class Reporter>
void print_errors(std::ostream& out, Reporter& err_rep) {
  for (const auto& error : err_rep->errors()) {
    out << fmt::format("{}", error);
  }
}
