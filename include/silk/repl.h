#pragma once

#include <istream>
#include <ostream>

struct Repl {
  private:
  void prompt(std::ostream& out) const noexcept;

  public:
  int run(std::istream& in, std::ostream& out) noexcept;
};
