#pragma once

#include "silk/util/cli.h"
#include <silk/compiler/compiler.h>
#include <silk/parser/ast.h>
#include <silk/util/error.h>

struct Debugger {
  private:
  // Prompts
  static constexpr auto PROMPT = YELLOW "$>" RESET " ";

  public:
  auto debug(AST&& ast, std::istream&, std::ostream&) noexcept -> void;
};
