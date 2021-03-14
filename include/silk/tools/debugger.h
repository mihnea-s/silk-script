#pragma once

#include <silk/compiler/compiler.h>
#include <silk/parser/ast.h>
#include <silk/util/cli.h>
#include <silk/util/error.h>

namespace silk {

/// TODoc
class Debugger {
private:
  // Prompts
  static constexpr auto PROMPT = YELLOW "$>" RESET " ";

public:
  /// TODoc
  auto debug(AST &&ast, std::istream &, std::ostream &) noexcept -> int;
};

} // namespace silk
