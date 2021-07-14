#include <algorithm>
#include <filesystem>
#include <fstream>
#include <optional>
#include <regex>
#include <sstream>

#include <silk/pipeline/context_builder.h>
#include <silk/utility/cli.h>

namespace silk {

const auto ContextBuilder::import_regex = std::regex{
  R"regex((?:^|\n)[^\S\r\n]*use[^\S\r\n]*'([^\n]*)')regex",
  std::regex_constants::optimize,
};

auto ContextBuilder::resolve_path(std::string import_string)
  -> std::optional<fs::path> {
  const auto suffix = import_string + ".silk";

  if (fs::is_regular_file(fs::current_path() / suffix)) {
    return fs::current_path() / suffix;
  }

  for (auto &include_path : include_paths) {
    const auto path = include_path / suffix;
    if (fs::is_regular_file(path)) return path;
  }

  report(fmt_function(
    "unable to resolve import '{}', not found in any include path",
    import_string));

  return std::nullopt;
}

auto ContextBuilder::resolve_imports(
  std::unordered_map<std::string, Source> &imports, Source &file) -> void {
  auto content = [&] {
    auto buf = std::ostringstream{};
    buf << file.source.rdbuf();
    return buf.str();
  }();

  auto beg = std::sregex_iterator{content.begin(), content.end(), import_regex};
  auto end = std::sregex_iterator{};

  std::for_each(beg, end, [&](std::smatch match) {
    if (const auto resolved = resolve_path(match[1]); resolved) {
      const auto path = resolved.value();

      // Import already resolved
      if (imports.find(path) != imports.end()) return;

      // Recursively resolve imports of new import
      imports.emplace(path, Source{path, std::ifstream(path)});
      resolve_imports(imports, imports.at(path));
    }
  });

  // Reset file to start
  file.source.seekg(0);
}

auto ContextBuilder::execute(Source &&main_source) noexcept -> PackageSource {
  auto imported_sources = std::unordered_map<std::string, Source>{};
  resolve_imports(imported_sources, main_source);

  return {
    .main    = std::move(main_source),
    .sources = std::move(imported_sources),
  };
}

} // namespace silk
