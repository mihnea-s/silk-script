#include <string_view>

#include <fmt/core.h>
#include <fmt/format.h>

#include <silk/error.h>

#define RESET "\u001b[0m"
#define BOLD  "\u001b[1m"
#define YELW  "\u001b[33m"
#define RED   "\u001b[31m"

// extend fmt::format to be able to output
// silk's error types

template <>
struct fmt::formatter<Severity> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(Severity sev, FormatContext& ctx) {
    string_view msg = "unknown";

    switch (sev) {
      case Severity::error: {
        msg = BOLD RED "[!] error" RESET;
        break;
      }
      case Severity::warning: {
        msg = BOLD YELW "(*) warning" RESET;
        break;
      }
    }

    return fmt::format_to(ctx.out(), "{}", msg);
  }
};

auto print_error(const ParsingError& e) -> void {
  fmt::print(
    "{}:\n"                                        //
    BOLD "   where" RESET ": line {}, column {}\n" //
    BOLD "   what" RESET ": {}\n",                 //
    e.severity(),                                  //
    e.line(),                                      //
    e.column(),                                    //
    e.what()                                       //
  );
}

auto print_error(const RuntimeError& e) -> void {
  fmt::print(
    BOLD RED "[!] runtime error" RESET ":\n" //
    BOLD "   what" RESET ": {}\n",           //
    e.what()                                 //
  );
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