#include <map>
#include <string_view>

#include <fmt/core.h>

#include <silk/util/cli.h>
#include <silk/util/parameters.h>
#include <util/message.h>

constexpr auto param_help(Flag flag) -> std::string_view {
  switch (flag) {
    case Flag::HELP: return "shows the help page";
    case Flag::COMPILE: return "compile the files to a silk vm executable";
    case Flag::DEBUG: return "interpret the files with the debug tool";
    case Flag::INTERACTIVE: return "open a repl session";
    default: return "no clue bro";
  }
}

auto print_param(Flag flag) {
  fmt::print(
    "\t" BOLD "{}" RESET ", " BOLD "{}" RESET "  ->  {}\n",
    Parameters::usage(flag).first,  //
    Parameters::usage(flag).second, //
    param_help(flag)                //
  );
}

auto print_help() -> void {
  fmt::print(BLUE BOLD "(?) Help" RESET ": silk usage:\n");
  for (size_t fl = (size_t)Flag::HELP; fl != (size_t)Flag::LAST; fl++) {
    print_param((Flag)fl);
  }
}
