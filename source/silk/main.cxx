#include <fstream>
#include <iostream>

#include <fmt/format.h>

#include <moth/vm.h>
#include <silk/compiler/compiler.h>
#include <silk/compiler/type_checker.h>
#include <silk/lexer/lexer.h>
#include <silk/parser/parser.h>
#include <silk/tools/debugger.h>
#include <silk/tools/repl.h>
#include <silk/util/cli.h>
#include <silk/util/error.h>

int main(const int argc, const char** argv) {
  const auto flags = CLIFlags {argc, argv};

  if (flags.is_set(CLIFlags::HELP)) {
    std::cout << CLIFlags::help_string();
    return 0;
  }

  if (flags.is_set(CLIFlags::INTERACTIVE)) {
    auto repl = Repl {};
    return repl.run(std::cin, std::cout);
  }

  if (!flags.files().size()) {
    print_error(std::cout, "no files");
    return 1;
  }

  for (auto& file : flags.files()) {
    auto file_stream = std::ifstream(file);

    if (!file_stream) {
      print_error(std::cout, "file not found `{}`", file);
      return 1;
    }

    auto       lexer  = Lexer {};
    const auto tokens = lexer.scan(file_stream);

    auto parser = Parser {};
    auto ast    = parser.parse(begin(tokens), end(tokens));

    if (parser.has_error()) {
      print_errors(std::cout, parser);
      return 1;
    }

    auto checker = TypeChecker {};
    checker.type_check(ast);

    if (checker.has_error()) {
      print_errors(std::cout, checker);
      return 1;
    }

    if (flags.is_set(CLIFlags::DEBUG)) {
      // debug using the debugger
      auto debugger = Debugger {};
      debugger.debug(std::move(ast), std::cin, std::cout);

    } else {
      // compile to vm bytecode
      auto compiler = Compiler {};
      compiler.compile(ast);

      if (compiler.has_error()) {
        print_errors(std::cout, compiler);
        return 1;
      }

      if (flags.is_set(CLIFlags::COMPILE)) {
        compiler.write_to_file(fmt_function("{}.silkexe", file));

        if (compiler.has_error()) {
          print_errors(std::cout, compiler);
          return 1;
        }
      }

      if (flags.is_set(CLIFlags::RUN)) {
        auto vm = [] {
          VM vm;
          init_vm(&vm);
          return vm;
        }();

        auto result = compiler.run_in_vm(&vm);

        free_vm(&vm);

        return result;
      };
    }
  }

  return 0;
}
