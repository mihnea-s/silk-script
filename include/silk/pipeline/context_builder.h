#pragma once

#include <filesystem>
#include <initializer_list>
#include <regex>
#include <type_traits>
#include <vector>

#include <silk/language/package.h>
#include <silk/pipeline/stage.h>

namespace silk {

namespace fs = std::filesystem;

class ContextBuilder final :
    public NonSyntaxTreeStage<ContextBuilder, Source, PackageSource> {

private:
  static const std::regex import_regex;

  const std::vector<fs::path> include_paths;

  auto resolve_path(std::string) -> std::optional<fs::path>;
  auto resolve_imports(
    std::unordered_map<std::string, Source> &imports, Source &file) -> void;

public:
  ContextBuilder(std::vector<fs::path> &&include_paths) :
      include_paths(include_paths) {
  }

  ~ContextBuilder() {
  }

  ContextBuilder(const ContextBuilder &) = delete;
  ContextBuilder(ContextBuilder &&)      = default;

  auto execute(Source &&) noexcept -> PackageSource override;
};

} // namespace silk
