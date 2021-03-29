#pragma once

#include <silk/language/package.h>
#include <silk/pipeline/stage.h>
#include <silk/utility/cli.h>

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