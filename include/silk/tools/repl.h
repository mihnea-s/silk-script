#pragma once

#include <istream>
#include <ostream>

#include <silk/tools/cli.h>

namespace silk {

// TODO: implement
class Repl {
private:
  static constexpr auto PROMPT       = CYAN "|>" RESET " ";
  static constexpr auto MLINE_PROMPT = CYAN "|:" RESET " ";

public:
  auto run(std::istream &in, std::ostream &out) noexcept -> int;
};

} // namespace silk
