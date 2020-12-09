#include <silk/tools/debugger.h>

#include "moth/disas.h"
#include "moth/vm.h"
#include "silk/util/error.h"
#include <cstddef>
#include <memory>

#include <string>
#include <string_view>
#include <utility>
#include <vector>

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

    YELLOW "\tp " RESET BOLD "<name> " RESET "- print value of a variable\n"

    YELLOW "\tb " RESET BOLD "<file>:<line> " RESET
              "- break at line of source file\n"

    YELLOW "\tf " RESET BOLD "<name> " RESET
              "- break when function gets called\n";
}

auto Debugger::debug(AST &&ast, std::istream &in, std::ostream &out) noexcept
  -> int {
  // Compile the program
  auto compiler = Compiler{};
  compiler.compile(ast);

  if (compiler.has_error()) {
    print_errors(out, compiler);
    return 1;
  }

  // The execution vm
  auto vm = VM{};
  init_vm(&vm);

  // make sure the vm gets free'd
  auto __ = std::shared_ptr<void>{nullptr, [&](...) { free_vm(&vm); }};

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
        vm_run(&vm, &compiler.bytecode());
        break;
      }

      case 'd': {
        disassemble("PROGRAM", &compiler.bytecode());
        break;
      }

      case 'p': break;
      case 'b': break;
      case 'f': break;

      case '\0': break;
      default: print_error(out, "invalid command `{}`", cmd);
    }
  }

  return 0;
}
