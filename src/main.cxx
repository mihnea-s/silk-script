#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <fmt/core.h>
#include <fmt/format.h>

#include <vm/chunk.h>
#include <vm/file_exec.h>

#include <silk/common/error.h>
#include <silk/common/lexer.h>
#include <silk/compiler/compiler.h>
#include <silk/interpreter/repl.h>
#include <silk/util/cli.h>
#include <silk/util/filepath.h>
#include <silk/util/parameters.h>

int main(const int argc, const char** argv) {
  if (argc < 2) {
    auto repl = Repl {};
    return repl.run(std::cin, std::cout);
  }

  auto params = Parameters {argc, argv};

  if (params.flag(Flag::HELP)) {
    print_help();
    return 0;
  }

  for (auto& file : params.files()) {
    auto file_stream = std::ifstream(file);

    if (!file_stream) {
      print_error("file not found '{}'", file);
      return 1;
    }

    auto tokens = Lexer {}.scan(file_stream);
    auto chunks = Compiler {}.compile(begin(tokens), end(tokens));

    auto file_name = get_file_name(file);

    const char* error = nullptr;
    write_file(file_name.c_str(), chunks.data(), chunks.size(), &error);

    if (error) {
      print_error(error);
      return 1;
    }

    for (auto& cnk : chunks) {
      free_chunk(&cnk);
    }
  }

  return 0;
}
