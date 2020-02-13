#pragma once

#include <istream>
#include <ostream>
#include <stack>

struct Repl {
  private:
  enum class NestingType { PARENTHESIS, SQUARE, BRACKETS };
  std::stack<NestingType> _nesting;

  // check nesting levels
  bool is_nested(std::string_view str) noexcept;

  // prompts
  const char* prompt() const noexcept;
  const char* multiline_prompt() const noexcept;

  public:
  int run(std::istream& in, std::ostream& out) noexcept;
};
