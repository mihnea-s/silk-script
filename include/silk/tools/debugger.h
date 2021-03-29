#pragma once

#include <silk/language/syntax_tree.h>
#include <silk/pipeline/stage.h>
#include <silk/utility/cli.h>

namespace silk {

// TODO: implement
class Debugger {
private:
  static constexpr auto PROMPT = YELLOW "$>" RESET " ";

public:
  auto debug(Package &&, std::istream &, std::ostream &) noexcept -> int;
};

} // namespace silk
