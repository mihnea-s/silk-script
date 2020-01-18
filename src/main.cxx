#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <fmt/core.h>
#include <fmt/format.h>

#include <silk/common/error.h>
#include <silk/interpreter/repl.h>

int main(const int argc, const char** argv) {
  if (argc < 2) {
    auto repl = Repl {};
    std::exit(repl.run(std::cin, std::cout));
  }

  auto files = std::vector<std::string> {argv + 1, argv + argc};

  for (auto& file : files) {
    auto file_stream = std::ifstream(file);

    if (!file_stream) {
      print_error("file not found '{}'", file);
      std::exit(1);
    }
  }

  return 0;
}
