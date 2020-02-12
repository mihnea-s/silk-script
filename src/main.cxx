
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <fmt/core.h>
#include <fmt/format.h>

#include <silk/common/error.h>
#include <silk/common/lexer.h>
#include <silk/compiler/compiler.h>
#include <silk/interpreter/analyzers/checker.h>
#include <silk/interpreter/analyzers/parser.h>
#include <silk/interpreter/repl.h>
#include <silk/interpreter/runtime/interpreter.h>
#include <silk/util/cli.h>
#include <silk/util/filepath.h>
#include <silk/util/parameters.h>

int main(const int argc, const char** argv) {
  auto params = Parameters {argc, argv};

  if (params.flag(Flag::HELP)) {
    print_help();
    return 0;
  }

  if (params.flag(Flag::INTERACTIVE)) {
    auto repl = Repl {};
    return repl.run(std::cin, std::cout);
  }

  if (!params.files().size()) {
    print_error("no files");
    return 1;
  }

  for (auto& file : params.files()) {
    auto file_stream = std::ifstream(file);

    if (!file_stream) {
      print_error("file not found '{}'", file);
      return 1;
    }

    const auto tokens = Lexer {}.scan(file_stream);

    if (params.flag(Flag::DEBUG)) {
      // debug using interpreter
      auto parser = Parser {};
      auto ast    = parser.parse(begin(tokens), end(tokens));

      if (parser.has_error()) {
        for (const auto& error : parser.errors()) {
          std::cout << fmt::format("{}", error);
        }

        return 1;
      }

      auto checker = Checker {};
      checker.check(ast);

      if (checker.has_error()) {
        for (const auto& error : checker.errors()) {
          std::cout << fmt::format("{}", error);
        }

        return 1;
      }

      auto interpreter = Interpreter {};
      interpreter.interpret(ast, std::cin, std::cout);

    } else {
      // compile to vm bytecode
      auto compiler = Compiler {};
      compiler.compile(begin(tokens), end(tokens));

      if (compiler.has_error()) {
        for (const auto& error : compiler.errors()) {
          std::cout << fmt::format("{}", error);
        }

        return 1;
      }

      const auto file_name = get_file_name(file);
      compiler.write_to_file(file_name);

      if (compiler.has_error()) {
        for (const auto& error : compiler.errors()) {
          std::cout << fmt::format("{}", error);
        }

        return 1;
      }
    }
  }

  return 0;
}
