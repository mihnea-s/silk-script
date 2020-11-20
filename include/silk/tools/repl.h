#pragma once

#include <istream>
#include <ostream>

#include <silk/util/cli.h>

class Repl {
private:
  // Prompts
  static constexpr auto PROMPT       = CYAN "|>" RESET " ";
  static constexpr auto MULTI_PROMPT = CYAN "|:" RESET " ";

  // check nesting levels
  int  _nesting = 0;
  auto is_nested(std::string_view str) noexcept -> bool;

public:
  auto run(std::istream &in, std::ostream &out) noexcept -> int;
};
