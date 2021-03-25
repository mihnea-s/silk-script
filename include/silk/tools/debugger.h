#pragma once

#include <silk/stages/pipeline.h>
#include <silk/syntax/tree.h>
#include <silk/tools/cli.h>

namespace silk {

// TODO: implement
class Debugger {
private:
  static constexpr auto PROMPT = YELLOW "$>" RESET " ";

public:
  auto debug(Package &&, std::istream &, std::ostream &) noexcept -> int;
};

} // namespace silk
