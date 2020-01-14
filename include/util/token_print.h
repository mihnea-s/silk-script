#include <fmt/core.h>
#include <string>
#include <string_view>

#include <silk/ast/token.h>
#include <silk/lexer.h>

#include "tok2str.h"

void token_print(const std::vector<Token>& tokens) {
  for (auto& token : tokens) {
    fmt::print("[{}]", tok2str(token));
  }

  fmt::print("\n");
}
