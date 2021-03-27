#include "silk/stages/pipeline.h"
#include "silk/syntax/tree.h"
#include <fstream>
#include <iostream>

#include <fmt/format.h>

#include <silk/stages/optimizer.h>
#include <silk/stages/parser.h>
#include <silk/stages/type_checker.h>
#include <silk/syntax/package.h>
#include <silk/tools/cli.h>
#include <silk/tools/debugger.h>
#include <silk/tools/repl.h>
#include <silk/stages/json_serializer.h>
#include <type_traits>

int main(const int argc, const char **argv) {
  const auto flags = silk::CLIFlags{argc, argv};

  if (flags.is_set(silk::CLIFlags::HELP)) {
    std::cout << silk::CLIFlags::help_string();
    return 0;
  }

  if (flags.is_set(silk::CLIFlags::INTERACTIVE)) {
    auto repl = silk::Repl{};
    return repl.run(std::cin, std::cout);
  }

  if (!flags.files().size()) {
    silk::print_error(std::cout, "no files");
    return 1;
  }

  auto pipeline = silk::Parser{} >> silk::TypeChecker{} >> silk::Optimizer{} >>
                  silk::JsonSerializer{};

  for (auto &file_name : flags.files()) {
    auto file = std::ifstream(file_name);

    if (!file) {
      silk::print_error(std::cout, "file not found `{}`", file_name);
      return 1;
    }

    auto source = silk::Source{file_name, file};
    auto result = pipeline.execute(std::move(source));

    if (pipeline.has_errors()) {
      std::cerr << "pipeline errors." << std::endl;
    } else {
      std::cout << result;
    }
  }

  return 0;
}
