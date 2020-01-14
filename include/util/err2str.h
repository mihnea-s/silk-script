#pragma once

#include <string_view>

#include <fmt/core.h>

#include <silk/error.h>

#define RC "\u001b[0m"
#define BD "\u001b[1m"

std::string_view sev2str(Severity sev) {
  switch (sev) {
    // clang-format off
    case Severity::compiler: return BD "\u001b[35m{#} compiler error" RC;
    case Severity::error:    return BD "\u001b[31m[!] error" RC;
    case Severity::warning:  return BD "\u001b[33m(*) warning" RC;
    case Severity::info:     return BD "\u001b[36m|i| info" RC;
    //  clang-format on

    default: {
      return "????";
    }
  }
}


void lexing_error_prtty_prnt(LexingError& e) {
  fmt::print("lexing {}: " BD "{}\n", e.severity(), e.what());
}

void parsing_error_prtty_prnt(ParsingError& e) {
  fmt::print(
    "{}:\n"                                   //
    BD "   where" RC ": line {}, column {}\n" //
    BD "   what" RC ": {}\n",                 //
    sev2str(e.severity()),
    e.line(),
    e.column(),
    e.what());
}

void runtime_error_prtty_prnt(RuntimeError& e) {
  fmt::print(
    BD "\u001b[31m[!] runtime error" RC ":\n"
    BD "   what" RC ": {}\n",
    e.what()
  );
}