#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <istream>
#include <iterator>
#include <numeric>
#include <ostream>
#include <string_view>

#include <fmt/core.h>
#include <fmt/format.h>

#include <silk/error.h>
#include <silk/lexer.h>
#include <silk/parser.h>
#include <silk/repl.h>
#include <silk/runtime/interpreter.h>

#define DBG_MODE 0
#include <util/ast_print.h>
#include <util/err2str.h>

int main(const int argc, const char** argv) {
  if (argc < 2) {
    auto repl = Repl {};
    std::exit(repl.run(std::cin, std::cout));
  }

  auto files = std::vector<std::string> {argv + 1, argv + argc};

  auto lexer       = Lexer {};
  auto parser      = Parser {};
  auto interpreter = Interpreter {};

  lexer.set_error_callback(lexing_error_prtty_prnt);
  parser.set_error_callback(parsing_error_prtty_prnt);
  interpreter.set_error_callback(runtime_error_prtty_prnt);

  for (auto& file : files) {
    auto file_stream = std::ifstream(file);

    if (!file_stream) {
      fmt::print("Error: file not found {}\n", file);
      std::exit(1);
    }

    auto& tokens = lexer.scan(file_stream);
    auto  ast    = parser.parse(begin(tokens), end(tokens));

#if DBG_MODE
    ast_print(ast);
#else
    interpreter.interpret(ast);
#endif
  }

  return 0;
}
