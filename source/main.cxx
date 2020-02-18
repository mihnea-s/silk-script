
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <fmt/core.h>
#include <fmt/format.h>

#include <silk/common/checker.h>
#include <silk/common/error.h>
#include <silk/common/lexer.h>
#include <silk/common/parser.h>
#include <silk/compiler/compiler.h>
#include <silk/debugger/debugger.h>
#include <silk/repl/repl.h>
#include <silk/util/cli.h>
#include <silk/util/filepath.h>
#include <silk/util/parameters.h>

template <class Reporter>
void handle_errors(Reporter* err_rep) {
  if (err_rep->has_warning()) {
    for (const auto& error : err_rep->errors()) {
      std::cout << fmt::format("{}", error);
    }

    if (err_rep->has_error()) {
      delete err_rep;
      std::exit(1);
    }
  }
}

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

  int return_value = 0;
  for (auto& file : params.files()) {
    auto file_stream = std::ifstream(file);

    if (!file_stream) {
      print_error("file not found '{}'", file);
      return 1;
    }

    const auto tokens = Lexer {}.scan(file_stream);

    auto parser = new Parser {};
    auto ast    = parser->parse(begin(tokens), end(tokens));
    handle_errors(parser);
    delete parser;

    auto checker = new Checker {};
    checker->check(ast);
    handle_errors(checker);
    delete checker;

    if (params.flag(Flag::DEBUG)) {
      // debug using the debugger
      auto debugger = Debugger {};
      debugger.debug(ast, std::cin, std::cout);
    } else {
      // compile to vm bytecode
      auto compiler = new Compiler {};
      compiler->compile(ast);
      handle_errors(compiler);

      if (params.flag(Flag::RUN)) {
        // run in the embedded vm
        return_value = compiler->run_in_vm();
      }

      if (params.flag(Flag::COMPILE) || !params.flag(Flag::RUN)) {
        // output to file
        const auto file_name = get_file_name(file);
        compiler->write_to_file(file_name);
        handle_errors(compiler);
      }

      delete compiler;
    }
  }

  return return_value;
}
