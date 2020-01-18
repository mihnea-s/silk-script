#include <cstddef>
#include <cstdlib>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>

#include <silk/common/error.h>
#include <silk/common/lexer.h>
#include <silk/interpreter/analyzers/checker.h>
#include <silk/interpreter/analyzers/parser.h>
#include <silk/interpreter/repl.h>
#include <silk/interpreter/runtime/interpreter.h>

#include <util/message.h>

constexpr const char* custom_prompt_env    = "SILK_REPL";
constexpr const char* custom_ml_prompt_env = "SILK_REPL_ML";

// helper function

template <class Analyzer>
void handleErrors(Analyzer& analyzer) {
  if (analyzer.has_error()) {
    for (auto& error : analyzer.errors()) {
      fmt::print("{}", error);
    }
    analyzer.clear_errors();
  }
}

// print prompt

bool Repl::isNested(std::string_view str) noexcept {
  for (auto& c : str) {
    switch (c) {
      case '(': _nesting.push(NestingType::parends); break;
      case '{': _nesting.push(NestingType::braces); break;
      case ')': _nesting.pop(); break;
      case '}': _nesting.pop(); break;
    }
  }

  return _nesting.size() != 0;
}

const char* Repl::prompt() const noexcept {
  auto custom_prompt = std::getenv(custom_prompt_env);
  if (custom_prompt) { return custom_prompt; }
  return CYAN "$>" RESET " ";
}

const char* Repl::multiline_prompt() const noexcept {
  auto custom_prompt = std::getenv(custom_ml_prompt_env);
  if (custom_prompt) { return custom_prompt; }
  return CYAN "$:" RESET " ";
}

int Repl::run(std::istream& in, std::ostream& out) noexcept {
  auto lexer       = Lexer {};
  auto parser      = Parser {};
  auto checker     = Checker {};
  auto interpreter = Interpreter {};

  auto line = std::string {};

  out << prompt();

  while (std::getline(in, line)) {
    auto line_stream = std::stringstream {};
    line_stream << line << std::endl;

    while (isNested(line)) {
      out << multiline_prompt();
      std::getline(in, line);
      line_stream << line << std::endl;
    }

    // scan
    auto tokens = lexer.scan(line_stream);

    // parse
    auto ast = parser.parse(begin(tokens), end(tokens));
    handleErrors(parser);

    // type checking
    // checker.check(ast);
    // handleErrors(checker);

    // finally execute the line
    interpreter.interpret(ast);
    handleErrors(interpreter);

    // print prompt
    out << prompt();
  }

  return 0;
}
