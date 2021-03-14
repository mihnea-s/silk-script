#pragma once

#define RESET "\033[00m"

#define BOLD      "\033[1m"
#define DARK      "\033[2m"
#define UNDERLINE "\033[4m"
#define BLINK     "\033[5m"
#define REVERSE   "\033[7m"
#define CONCEALED "\033[8m"
#define GRAY      "\033[30m"
#define GREY      "\033[30m"
#define RED       "\033[31m"
#define GREEN     "\033[32m"
#define YELLOW    "\033[33m"
#define BLUE      "\033[34m"
#define MAGENTA   "\033[35m"
#define CYAN      "\033[36m"
#define WHITE     "\033[37m"

#define BG_GRAY    "\033[40m"
#define BG_GREY    "\033[40m"
#define BG_RED     "\033[41m"
#define BG_GREEN   "\033[42m"
#define BG_YELLOW  "\033[43m"
#define BG_BLUE    "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN    "\033[46m"
#define BG_WHITE   "\033[47m"

#include <bitset>
#include <string_view>
#include <vector>

namespace silk {

/// TODoc
struct CLIFlags {
private:
  enum class Flag {
    HELP,
    COMPILE,
    DEBUG,
    INTERACTIVE,
    RUN,

    // used for iteration and counting
    // do not touch !
    LAST,
  };

  std::bitset<(size_t)Flag::LAST> _bits;
  std::vector<std::string>        _files;

  auto is_flag(const char *) const -> bool;
  auto is_flag(const char *, Flag) const -> bool;

  auto parse(const int, const char **) -> void;

public:
  /// TODoc
  static constexpr auto HELP = Flag::HELP;
  /// TODoc
  static constexpr auto COMPILE = Flag::COMPILE;
  /// TODoc
  static constexpr auto DEBUG = Flag::DEBUG;
  /// TODoc
  static constexpr auto INTERACTIVE = Flag::INTERACTIVE;
  /// TODoc
  static constexpr auto RUN = Flag::RUN;

  /// TODoc
  auto is_set(Flag) const -> bool;

  /// TODoc
  auto files() const -> const std::vector<std::string> &;

  /// TODoc
  CLIFlags(const int argc, const char **argv) : _bits() {
    parse(argc, argv);
  }

  /// TODoc
  static auto help_string() noexcept -> std::string;

  /// TODoc
  static constexpr auto flag_aliases(Flag flag)
    -> std::pair<std::string_view, std::string_view>;

  /// TODoc
  static constexpr auto flag_usage(Flag flag) -> std::string_view;
};

} // namespace silk
