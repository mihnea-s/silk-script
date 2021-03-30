#include <fstream>

#include <silk/tools/debugger.h>
#include <silk/tools/repl.h>
#include <silk/utility/cli.h>

#include <silk/pipeline/json_serializer.h>
#include <silk/pipeline/optimizer.h>
#include <silk/pipeline/parser.h>
#include <silk/pipeline/type_checker.h>

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
      silk::print_error(std::cerr, "file not found `{}`", file_name);
      return 1;
    }

    auto source = silk::Source{file_name, file};
    auto result = pipeline.execute(std::move(source));

    if (pipeline.has_errors()) {
      std::cerr << "pipeline errors." << std::endl;

      for (auto &&err : pipeline.errors()) {
        err.print(std::cerr);
      }
    } else {
      std::cout << result;
    }
  }

  return 0;
}
