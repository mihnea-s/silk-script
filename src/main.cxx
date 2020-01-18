#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <fmt/core.h>
#include <fmt/format.h>

#include <silk/common/error.h>
#include <silk/interpreter/repl.h>
#include <silk/util/cli.h>
#include <silk/util/parameters.h>

int main(const int argc, const char** argv) {
  if (argc < 2) {
    auto repl = Repl {};
    std::exit(repl.run(std::cin, std::cout));
  }

  auto params = Parameters {argc, argv};

  if (params.flag(Flag::HELP)) { print_help(); }

  for (auto& file : params.files()) {
    auto file_stream = std::ifstream(file);

    if (!file_stream) {
      print_error("file not found '{}'", file);
      std::exit(1);
    }
  }

  return 0;
}
