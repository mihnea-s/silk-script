#pragma once

#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include <silk/stages/pipeline.h>
#include <silk/syntax/tree.h>
#include <silk/tools/cli.h>

namespace silk {

class JsonDeserializer final :
    public NonSyntaxTreeStage<JsonDeserializer, std::string, Module> {
public:
  JsonDeserializer() {
  }

  ~JsonDeserializer() {
  }

  JsonDeserializer(const JsonDeserializer &) = delete;
  JsonDeserializer(JsonDeserializer &&)      = default;

  auto execute(std::string &&) noexcept -> Module override;
};

} // namespace silk