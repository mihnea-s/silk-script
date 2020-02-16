#pragma once

#include <string_view>

#include <fmt/format.h>

#ifdef CPP20
  #define SILKERRORCNST constexpr
#else
  #define SILKERRORCNST const
#endif

class SilkErrors {
  public:
  SILKERRORCNST static std::string invalidState() {
    return "invalid state";
  }

  SILKERRORCNST static std::string multiPkg() {
    return "multiple package declarations in the same file";
  }

  SILKERRORCNST static std::string
  expectedAfter(std::string_view expect, std::string_view after) {
    using namespace fmt::literals;
    return "expected `{}` after {}"_format(expect, after);
  }

  SILKERRORCNST static std::string expectedIdentif(std::string_view instead) {
    using namespace fmt::literals;
    return "expected an identifier, found '{}' instead"_format(instead);
  }

  SILKERRORCNST static std::string expectedType(std::string_view instead) {
    using namespace fmt::literals;
    return "expected a type, found '{}' instead"_format(instead);
  }

  SILKERRORCNST static std::string expectedStr(std::string_view instead) {
    using namespace fmt::literals;
    return "expected a string, found '{}' instead"_format(instead);
  }

  SILKERRORCNST static std::string stmtSemicolon() {
    return "missing semicolon after statement";
  }

  SILKERRORCNST static std::string missingComma() {
    return "missing comma between expressions";
  }

  SILKERRORCNST static std::string undefAssign(std::string_view what) {
    using namespace fmt::literals;
    return "assigment to undefined variable '{}'"_format(what);
  }

  SILKERRORCNST static std::string constAssign(std::string_view what) {
    using namespace fmt::literals;
    return "assigment to immutable variable '{}'"_format(what);
  }

  SILKERRORCNST static std::string undefUsage(std::string_view what) {
    using namespace fmt::literals;
    return "use of undefined variable '{}'"_format(what);
  }

  SILKERRORCNST static std::string invalidOperand(std::string_view op) {
    using namespace fmt::literals;
    return "invalid operand to {} operation"_format(op);
  }

  SILKERRORCNST static std::string noScope() {
    return "variable declared outside of main";
  }
};