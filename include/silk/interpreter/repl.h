#pragma once

#include <istream>
#include <ostream>
#include <stack>

struct Repl {
  private:
  enum class NestingType {
    parends,
    braces,
  };

  std::stack<NestingType> _nesting;

  bool isNested(std::string_view str) noexcept;

  const char* prompt() const noexcept;
  const char* multiline_prompt() const noexcept;

  public:
  int run(std::istream& in, std::ostream& out) noexcept;
};
