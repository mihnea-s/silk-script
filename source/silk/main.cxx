#include "silk/targets/wasm/compiler.h"
#include <fstream>

#include <iterator>
#include <silk/tools/debugger.h>
#include <silk/tools/repl.h>
#include <silk/utility/cli.h>

#include <silk/pipeline/context_builder.h>
#include <silk/pipeline/json_serializer.h>
#include <silk/pipeline/optimizer.h>
#include <silk/pipeline/parser.h>
#include <silk/pipeline/type_checker.h>
#include <silk/targets/moth/compiler.h>
#include <utility>

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

  auto file_paths = flags.files();

  if (!file_paths.size()) {
    silk::print_error(std::cout, "no files");
    return 1;
  }

  // Main source file is last in argument list
  auto main_path = file_paths.back();
  file_paths.pop_back();

  // Rest of the arguments are added to the include paths
  auto include_paths = std::vector<std::filesystem::path>{};
  std::move(begin(file_paths), end(file_paths), back_inserter(include_paths));

  // Create a compilation pipeline
  auto pipeline = silk::ContextBuilder{std::move(include_paths)} >>
                  silk::Parser{} >> silk::TypeChecker{} >> silk::Optimizer{} >>
                  silk::JsonSerializer{} // >> silk::moth::Compiler{}
  ;

  std::cout << pipeline.execute({
    .path   = main_path,
    .source = std::ifstream{main_path},
  });

  if (pipeline.has_errors()) {
    std::cerr << "pipeline errors." << std::endl;

    for (auto &&err : pipeline.errors()) {
      err.print(std::cerr);
    }
  }

  return 0;
}
