#pragma once

#include <istream>
#include <ostream>

#include <silk/util/cli.h>

namespace silk {

/// TODoc
class Repl {
private:
  static constexpr auto PROMPT       = CYAN "|>" RESET " ";
  static constexpr auto MLINE_PROMPT = CYAN "|:" RESET " ";

public:
  /// TODoc
  auto run(std::istream &in, std::ostream &out) noexcept -> int;
};

} // namespace silk
