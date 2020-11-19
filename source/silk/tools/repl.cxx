#include <silk/tools/repl.h>

#include <cstddef>
#include <cstdlib>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>

#include <moth/vm.h>
#include <silk/compiler/compiler.h>
#include <silk/compiler/type_checker.h>
#include <silk/lexer/lexer.h>
#include <silk/parser/parser.h>
#include <silk/util/cli.h>
#include <silk/util/error.h>

// print prompt

auto Repl::is_nested(std::string_view str) noexcept -> bool {
  for (auto& c : str) {
    switch (c) {
      case '(': [[fallthrough]];
      case '[': [[fallthrough]];
      case '{': _nesting++;

      case ')': [[fallthrough]];
      case ']': [[fallthrough]];
      case '}': _nesting--;
    }
  }

  // Quick sanity check
  if (_nesting < 0) _nesting = 0;

  return _nesting > 0;
}

auto Repl::run(std::istream& in, std::ostream& out) noexcept -> int {
  auto lexer    = Lexer {};
  auto parser   = Parser {};
  auto checker  = TypeChecker {};
  auto compiler = Compiler {};

  auto vm = VM {};
  init_vm(&vm);

  auto line = std::string {};

  out << PROMPT;

  while (std::getline(in, line)) {
    auto line_stream = std::stringstream {};
    line_stream << line << std::endl;

    while (is_nested(line)) {
      out << MULTI_PROMPT;
      std::getline(in, line);
      line_stream << line << std::endl;
    }

    // scan
    auto tokens = lexer.scan(line_stream);

    // parse
    auto ast = parser.parse(begin(tokens), end(tokens));
    if (parser.has_error()) {
      print_errors(out, parser);
      parser.clear_errors();
      continue;
    }

    // type checking
    checker.type_check(ast);
    if (checker.has_error()) {
      print_errors(out, checker);
      checker.clear_errors();
      continue;
    }

    // compile program
    compiler.compile(ast);
    if (compiler.has_error()) {
      print_errors(out, compiler);
      compiler.clear_errors();
      continue;
    }

    // finally execute the line
    compiler.run_in_vm(&vm);

    // print prompt
    out << PROMPT;
  }

  free_vm(&vm);
  return 0;
}
