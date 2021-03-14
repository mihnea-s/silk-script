#include "moth/disas.h"
#include <silk/tools/repl.h>

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>

#include <moth/vm.h>
#include <silk/compiler/compiler.h>
#include <silk/compiler/type_checker.h>
#include <silk/parser/parser.h>
#include <silk/util/cli.h>
#include <silk/util/error.h>

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
  auto vm = VM{};
  init_vm(&vm);

  while (!in.eof()) {
    auto input = std::stringstream{};

    out << PROMPT;
    read_line(in, input);

    while (is_nested(input.str())) {
      out << MLINE_PROMPT;
      read_line(in, input);
    }

    // parse
    auto parser = Parser{input};
    auto ast    = parser.parse_line();
    if (parser.has_error()) {
      print_errors(out, parser);
      parser.clear_errors();
      continue;
    }

    // type checking
    auto checker = TypeChecker{};
    checker.type_check(ast);
    if (checker.has_error()) {
      print_errors(out, checker);
      continue;
    }

    // compile program
    auto compiler = Compiler{};
    compiler.compile(ast);
    if (compiler.has_error()) {
      print_errors(out, compiler);
      continue;
    }

    disassemble("YOUR SHITTY PROGRAM", &compiler.bytecode());

    // finally execute the line
    vm_run(&vm, &compiler.bytecode());
    if (vm.st != STATUS_OK) {
      print_error(out, "vm failed with status {}", vm.st);
    }
  }

  free_vm(&vm);
  return 0;
}

} // namespace silk
