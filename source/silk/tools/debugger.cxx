#include <silk/tools/debugger.h>

#include <moth/disas.h>
#include <moth/vm.h>

namespace silk {

auto parse_command(const std::string &line)
  -> std::pair<char, std::string_view> {
  if (line.length() < 1) return {'\0', {}};
  return {line[0], std::string_view{line}.substr(1)};
}

constexpr auto help_string() -> std::string_view {
  return BOLD "Debugger Commands\n" RESET

    YELLOW "\th " RESET "- print help screen\n"

    YELLOW "\ts " RESET "- stop debugging\n"

    YELLOW "\tr " RESET "- begin running the program\n"

    YELLOW "\td " RESET "- disassemble immediate instructions\n"

    YELLOW "\tp " RESET "- print the vm's stack and environment\n"

    YELLOW "\tm " RESET "- break on main\n"

    YELLOW "\tb " RESET BOLD "<file>:<line> " RESET
              "- break at line of source file\n"

    YELLOW "\tf " RESET BOLD "<name> " RESET
              "- break when function gets called\n";
}

auto Debugger::debug(
  Package &&pkg, std::istream &in, std::ostream &out) noexcept -> int {

  while (!in.eof()) {
    auto line = std::string{};

    out << PROMPT;
    std::getline(in, line);

    const auto [cmd, args] = parse_command(line);

    switch (cmd) {
      case 'h': {
        out << help_string();
        break;
      }

      case 's': {
        return 0;
      }

      case 'r': {
        break;
      }

      case 'd': {
        break;
      }

      case 'p': {
        break;
      }

      case 'm': {
        break;
      }

      case 'b': {
        auto line = size_t{0};
        auto ss   = std::stringstream{std::string{args}};
        ss >> line;

        break;
      }

      case 'f': {
        break;
      }

      case '\0': break;
      default: print_error(out, "invalid command `{}`", cmd);
    }
  }

  return 0;
}

} // namespace silk
