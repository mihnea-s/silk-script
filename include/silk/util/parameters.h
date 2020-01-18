#pragma once

#include <bitset>
#include <map>
#include <string>
#include <string_view>
#include <vector>

enum class Flag {
  HELP,
  COMPILE,
  INTERACTIVE,

  // used for iteration and counting
  // do not touch !
  LAST,
};

struct Parameters {
  private:
  std::bitset<(size_t)Flag::LAST> _bits;
  std::vector<std::string>        _files;

  auto is_param(const char*) const -> bool;
  auto is_of_param(const char*, Flag) const -> bool;
  auto parse(const int, const char**) -> void;

  public:
  auto flag(Flag) const -> bool;
  auto files() const -> const std::vector<std::string>&;

  Parameters(const int argc, const char** argv) : _bits() {
    parse(argc, argv);
  }

  static constexpr auto usage(Flag flag)
    -> std::pair<std::string_view, std::string_view> {
    switch (flag) {
      case Flag::HELP: return {"-h", "--help"};
      case Flag::COMPILE: return {"-c", "--compile"};
      case Flag::INTERACTIVE: return {"-i", "--interactive"};
      default: return {"?", "?"};
    }
  }
};
