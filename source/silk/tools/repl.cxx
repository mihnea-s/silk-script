#include <silk/tools/repl.h>

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>

namespace silk {

// print prompt

inline auto read_line(std::istream &in, std::ostream &out) -> void {
  auto buf = std::string{};
  std::getline(in, buf);
  out << buf;
}

constexpr auto is_nested(std::string_view str) noexcept -> bool {
  auto nesting = 0;

  for (const auto &c : str) {
    switch (c) {
      case '(': [[fallthrough]];
      case '[': [[fallthrough]];
      case '{': nesting++; break;

      case ')': [[fallthrough]];
      case ']': [[fallthrough]];
      case '}': nesting--; break;
    }
  }

  return nesting > 0;
}

auto Repl::run(std::istream &in, std::ostream &out) noexcept -> int {
  while (!in.eof()) {
    auto input = std::stringstream{};

    out << PROMPT;
    read_line(in, input);

    while (is_nested(input.str())) {
      out << MLINE_PROMPT;
      read_line(in, input);
    }

    out << "Unfortunately, this compiler made in bosnia." << std::endl
        << "Due to poor technology in my country it do not" << std::endl
        << "work. Please imagine it work. Thank you." << std::endl;
  }

  return 0;
}

} // namespace silk
