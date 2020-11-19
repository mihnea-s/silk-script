#include "moth/vm.h"
#include "silk/util/error.h"
#include <silk/tools/debugger.h>

#include <string>
#include <string_view>
#include <utility>
#include <vector>

auto parse_command(const std::string& line)
  -> std::pair<char, std::string_view> {
  if (line.length() < 1) return {'\0', {}};
  return {line[0], std::string_view {line}.substr(1)};
}

constexpr auto help_string() -> std::string_view {
  return                                                                   //
    YELLOW "h " RESET "- print help screen\n"                              //
    YELLOW "s " RESET "- stop execution of the program\n"                  //
    YELLOW "r " RESET "- begin running the program\n"                      //
    YELLOW "d " RESET "- disassemble immediate instructions\n"             //
    YELLOW "p " RESET BOLD "<expr> " RESET "- print value of expression\n" //

    YELLOW "b " RESET BOLD "<file>:<line> " RESET
           "- break at line of source file\n" //

    YELLOW "f " RESET BOLD "<name> " RESET
           "- break when function gets called\n" //
    ;
}

auto Debugger::debug(AST&& ast, std::istream& in, std::ostream& out) noexcept
  -> void {
  // The execution vm
  auto vm = VM {};
  init_vm(&vm);

  auto line = std::string {};

  out << PROMPT;

  while (std::getline(in, line)) {
    const auto [cmd, args] = parse_command(line);

    switch (cmd) {
      case 'h': {
        out << help_string();
        break;
      }

      case 's': {
        free_vm(&vm);
        return;
      }

      case 'r': break;
      case 'd': break;
      case 'p': break;
      case 'b': break;
      case 'f': break;

      default: print_error(out, "invalid command `{}`", cmd);
    }
  }
}
