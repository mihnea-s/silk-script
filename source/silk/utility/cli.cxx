#include <silk/utility/cli.h>

#include <iostream>
#include <map>
#include <sstream>
#include <string_view>

namespace silk {

auto CLIFlags::is_flag(const char *arg) const -> bool {
  return strncmp(arg, "-", 1) == 0;
}

auto CLIFlags::is_flag(const char *arg, Flag flag) const -> bool {
  const auto [beg, end]       = std::pair{arg, arg + std::strlen(arg)};
  const auto [alias1, alias2] = flag_aliases(flag);

  return std::equal(beg, end, std::begin(alias1), std::end(alias1)) ||
         std::equal(beg, end, std::begin(alias2), std::end(alias2));
}

auto CLIFlags::files() const -> const std::vector<std::string> & {
  return _files;
}

auto CLIFlags::is_set(Flag flag) const -> bool {
  return _bits.test((size_t)flag);
}

auto CLIFlags::parse(const int argc, const char **argv) -> void {
  const char **arg = argv + 1;
  const char **end = argv + argc;

  for (; arg < end; arg++) {
    if (!is_flag(*arg)) {
      _files.push_back(*arg);
      continue;
    }

    auto param_found = false;

    for (size_t fl = (size_t)Flag::HELP; fl != (size_t)Flag::LAST; fl++) {
      if (is_flag(*arg, (Flag)fl)) {
        _bits.set(fl);
        param_found = true;
        break;
      }
    }

    if (!param_found) {
      print_error(std::cout, "invalid parameter '{}'", *arg);
    }
  }
}

auto CLIFlags::help_string() noexcept -> std::string {
  auto oss = std::ostringstream{};

  oss << fmt_function(BLUE BOLD "(?) Help" RESET ": silk usage:\n");

  for (size_t fl = (size_t)Flag::HELP; fl != (size_t)Flag::LAST; fl++) {
    const auto [alias1, alias2] = CLIFlags::flag_aliases((Flag)fl);

    oss << fmt_function(
      "\t" BOLD "{}" RESET ", " BOLD "{}" RESET "  ->  {}\n",
      alias1,
      alias2,
      flag_usage((Flag)fl));
  }

  return oss.str();
}

constexpr auto CLIFlags::flag_aliases(Flag flag)
  -> std::pair<std::string_view, std::string_view> {
  switch (flag) {
    case Flag::HELP: return {"-h", "--help"};
    case Flag::COMPILE: return {"-c", "--compile"};
    case Flag::WASM: return {"-w", "--wasm"};
    case Flag::JAVASCRIPT: return {"-j", "--js"};
    case Flag::RUN: return {"-r", "--run"};
    case Flag::DEBUG: return {"-d", "--debug"};
    case Flag::INTERACTIVE: return {"-i", "--interactive"};
    default: return {"?", "?"};
  }
}

constexpr auto CLIFlags::flag_usage(Flag flag) -> std::string_view {
  switch (flag) {
    case Flag::HELP: return "shows the help page";
    case Flag::COMPILE: return "compile the source to a moth executable";
    case Flag::WASM: return "compile the source to a wasm binary";
    case Flag::JAVASCRIPT: return "transpile the source to a js bundle";
    case Flag::DEBUG: return "debug the files with the debug tool";
    case Flag::INTERACTIVE: return "open a repl session";
    case Flag::RUN: return "compile and run the source (default behaviour)";
    default: return "error! this should never happen!";
  }
}

} // namespace silk
