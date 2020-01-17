#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <fmt/core.h>
#include <fmt/format.h>

#include <silk/analyzers/checker.h>
#include <silk/analyzers/parser.h>
#include <silk/error.h>
#include <silk/lexer.h>
#include <silk/repl.h>
#include <silk/runtime/interpreter.h>

template <class Analyzer>
void handleErrors(Analyzer& analyzer) {
  if (analyzer.has_error()) {
    for (auto& error : analyzer.errors()) {
      print_error(error);
      std::exit(1);
    }
  }
}

void runFile(std::ifstream& file) {
  auto lexer       = Lexer {};
  auto parser      = Parser {};
  auto checker     = Checker {};
  auto interpreter = Interpreter {};

  auto& tokens = lexer.scan(file);

  auto ast = parser.parse(begin(tokens), end(tokens));
  handleErrors(parser);

  // checker.check(ast);
  // handleErrors(checker);

  interpreter.interpret(ast);

  if (interpreter.has_error()) {
    for (auto& error : interpreter.errors()) {
      print_error(error);
      std::exit(1);
    }
  }
}

int main(const int argc, const char** argv) {
  if (argc < 2) {
    auto repl = Repl {};
    std::exit(repl.run(std::cin, std::cout));
  }

  auto files = std::vector<std::string> {argv + 1, argv + argc};

  for (auto& file : files) {
    auto file_stream = std::ifstream(file);

    if (!file_stream) {
      fmt::print("Error: file not found {}\n", file);
      std::exit(1);
    }

    runFile(file_stream);
  }

  return 0;
}
