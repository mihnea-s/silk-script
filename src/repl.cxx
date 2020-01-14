#include <cstddef>
#include <cstdlib>
#include <ostream>
#include <sstream>
#include <string>

#include <silk/lexer.h>
#include <silk/parser.h>
#include <silk/repl.h>
#include <silk/runtime/interpreter.h>

#define COLOR "\u001b[36m"
#define RESET "\u001b[0m"

void Repl::prompt(std::ostream& out) const noexcept {
  auto custom_prompt = std::getenv("SILK_REPL");

  if (custom_prompt) {
    out << custom_prompt << " ";
    return;
  }

  out << COLOR << "$" << RESET << " ";
}

int Repl::run(std::istream& in, std::ostream& out) noexcept {
  auto interpreter = Interpreter {};

  auto line = std::string {};

  prompt(out);

  while (std::getline(in, line)) {
    Lexer  lexer {};
    Parser parser {};

    auto ss     = std::istringstream {line};
    auto tokens = lexer.scan(ss);
    auto ast    = parser.parse(begin(tokens), end(tokens));

    ast.evaluate_with(interpreter);

    prompt(out);
  }

  return 0;
}
