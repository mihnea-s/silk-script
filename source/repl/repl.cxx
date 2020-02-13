#include <cstddef>
#include <cstdlib>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>

#include <silk/common/checker.h>
#include <silk/common/error.h>
#include <silk/common/lexer.h>
#include <silk/common/parser.h>
#include <silk/debugger/debugger.h>
#include <silk/repl/repl.h>

#include <util/message.h>

constexpr const char* custom_prompt_env    = "SILK_REPL";
constexpr const char* custom_ml_prompt_env = "SILK_REPL_ML";

// helper function

template <class Analyzer>
void handle_errors(Analyzer& analyzer, std::ostream& out) {
  if (analyzer.has_error()) {
    for (auto& error : analyzer.errors()) {
      out << fmt::format("{}", error);
    }
    analyzer.clear_errors();
  }
}

// print prompt

bool Repl::is_nested(std::string_view str) noexcept {
  for (auto& c : str) {
    switch (c) {
      case '(': _nesting.push(NestingType::PARENTHESIS); break;
      case '[': _nesting.push(NestingType::SQUARE); break;
      case '{': _nesting.push(NestingType::BRACKETS); break;

      case ')': _nesting.pop(); break;
      case ']': _nesting.pop(); break;
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
  auto lexer    = Lexer {};
  auto parser   = Parser {};
  auto checker  = Checker {};
  auto debugger = Debugger {};

  auto line = std::string {};

  out << prompt();

  while (std::getline(in, line)) {
    if (line.size() > 1 && line.at(0) == '/') {
      switch (line.at(1)) {
        case 'q': return 0;
        case 'p': out << line.substr(3); break;
      }

      continue;
    }

    auto line_stream = std::stringstream {};
    line_stream << line << std::endl;

    while (is_nested(line)) {
      out << multiline_prompt();
      std::getline(in, line);
      line_stream << line << std::endl;
    }

    // scan
    auto tokens = lexer.scan(line_stream);

    // parse
    auto ast = parser.parse(begin(tokens), end(tokens));
    handle_errors(parser, out);

    // type checking
    checker.check(ast);
    handle_errors(checker, out);

    // finally execute the line
    debugger.debug(ast, in, out);
    handle_errors(debugger, out);

    // print prompt
    out << prompt();
  }

  return 0;
}
