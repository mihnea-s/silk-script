#pragma once

#include <sstream>

#include <fmt/format.h>

#include <silk/util/cli.h>
#include <utility>

#define fmt_function fmt::format

namespace silk {

using Location = std::pair<std::uint64_t, std::uint64_t>;

/// TODoc
struct SilkError : std::exception {
private:
  const Location    _location;
  const std::string _message;

public:
  /// TODoc
  SilkError(const std::string message, const Location location) noexcept :
      _location(std::move(location)), _message(std::move(message)) {
  }

  /// TODoc
  std::uint64_t line() const noexcept;
  /// TODoc
  std::uint64_t column() const noexcept;

  /// TODoc
  const char *what() const noexcept override;
};

/// TODoc
class ErrorReporter {
protected:
  std::vector<SilkError> mutable _errors;

  template <class... Args>
  auto report(Location loc, std::string_view frmt, Args... args) const {
    _errors.emplace_back(fmt_function(frmt, std::forward<Args>(args)...), loc);
    return _errors.back();
  }

public:
  virtual ~ErrorReporter() {
  }

  /// TODoc
  auto has_error() const -> bool;

  /// TODoc
  auto clear_errors() -> void;

  /// TODoc
  auto errors() const -> const std::vector<SilkError> &;
};

/// TODoc
/// Format and then pretty print an error to `out`,
/// the formatting is the same as that of `fmt::format`
template <class... Args>
void print_error(std::ostream &out, std::string_view frmt, Args... args) {
  out << BOLD RED "(!) Error" RESET ":" << std::endl;
  out << "\t" << BOLD "what" RESET ": "
      << fmt_function(frmt, std::forward<Args>(args)...) << RESET << std::endl;
}

/// TODoc
/// Pretty print errors from an instance of an
/// [`ErrorReporter`] to `out`
template <class Reporter>
void print_errors(std::ostream &out, Reporter &err_rep) {
  for (const auto &error : err_rep.errors()) {
    out << fmt_function("{}", error);
  }
}

} // namespace silk

namespace fmt {

// extend format to be able to output
// silk's error type

template <>
struct formatter<silk::SilkError> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext &ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const silk::SilkError &err, FormatContext &ctx) {
    std::ostringstream msg{};

    msg << BOLD RED "(!) Error" RESET ":" << std::endl;

    msg << "\t" << BOLD "where" RESET ": line " BLUE << err.line()
        << RESET ", column " BLUE << err.column() << RESET << std::endl;

    msg << "\t" << BOLD "what" RESET ": " << err.what() << RESET << std::endl;

    return format_to(ctx.out(), "{}", msg.str());
  }
};

} // namespace fmt
