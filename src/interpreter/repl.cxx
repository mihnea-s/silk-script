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

bool continuesOnNextLine(std::string_view sv) {
  return (*sv.rbegin()) == '{' || (*sv.rbegin()) == '(';
}

bool endedOnCurrentLine(std::string_view sv) {
  return (*sv.rbegin()) == '}' || (*sv.rbegin()) == ')';
}

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

void Repl::prompt(std::ostream& out) const noexcept {
  auto custom_prompt = std::getenv(custom_prompt_env);

  if (custom_prompt) {
    out << custom_prompt << " ";
    return;
  }

  out << CYAN << "$>" << RESET << " ";
}

void Repl::multiline_prompt(std::ostream& out) const noexcept {
  auto custom_prompt = std::getenv(custom_ml_prompt_env);

  if (custom_prompt) {
    out << custom_prompt << " ";
    return;
  }

  out << CYAN << "$:" << RESET << "\t";
}

int Repl::run(std::istream& in, std::ostream& out) noexcept {
  auto lexer       = Lexer {};
  auto parser      = Parser {};
  auto checker     = Checker {};
  auto interpreter = Interpreter {};

  auto line = std::string {};

  // first prompt
  prompt(out);

  while (std::getline(in, line)) {
    // for the lexer
    auto line_stream = std::stringstream {line};

    if (continuesOnNextLine(line)) {
      while (!endedOnCurrentLine(line)) {
        multiline_prompt(out);
        std::getline(in, line);
        line_stream << line;
      }
    }

    // scan
    auto tokens = lexer.scan(line_stream);

    // parse
    auto ast = parser.parse(begin(tokens), end(tokens));
    handleErrors(parser);

    // type checking
    checker.check(ast);
    handleErrors(checker);

    // finally execute the line
    interpreter.interpret(ast);
    handleErrors(interpreter);

    // reprint prompt
    prompt(out);
  }

  return 0;
}
