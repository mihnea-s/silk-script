#include <cstddef>
#include <cstdlib>
#include <ostream>
#include <sstream>
#include <string>

#include <silk/analyzers/parser.h>
#include <silk/lexer.h>
#include <silk/repl.h>
#include <silk/runtime/interpreter.h>

#define COLOR "\u001b[36m"
#define RESET "\u001b[0m"

constexpr const char* custom_prompt_env = "SILK_REPL";

void Repl::prompt(std::ostream& out) const noexcept {
  auto custom_prompt = std::getenv(custom_prompt_env);

  if (custom_prompt) {
    out << custom_prompt << " ";
    return;
  }

  out << COLOR << "$" << RESET << " ";
}

int Repl::run(std::istream& in, std::ostream& out) noexcept {
  auto lexer       = Lexer {};
  auto parser      = Parser {};
  auto interpreter = Interpreter {};

  auto line = std::string {};

  // first prompt
  prompt(out);

  while (std::getline(in, line)) {
    // for the lexer
    auto line_stream = std::istringstream {line};

    // scan & parse
    auto tokens = lexer.scan(line_stream);
    auto ast    = parser.parse(begin(tokens), end(tokens));

    // finally execute the line
    ast.execute_with(interpreter);

    // reprint prompt
    prompt(out);
  }

  return 0;
}
