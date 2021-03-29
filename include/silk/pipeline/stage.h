#pragma once

#include <silk/language/package.h>
#include <silk/language/token.h>

namespace silk {

class Error : public std::exception {
protected:
  const std::string _message;

public:
  Error(const std::string message) noexcept : _message(std::move(message)) {
  }

  virtual ~Error() {
  }

  virtual void print(std::ostream &) const noexcept;

  const char *what() const noexcept override;
};

class LocError : public Error {
protected:
  const Location _location;

public:
  LocError(const Location location, const std::string message) noexcept :
      Error(std::move(message)), _location(std::move(location)) {
  }

  virtual ~LocError() {
  }

  virtual void print(std::ostream &) const noexcept override;
};

template <class T>
constexpr auto is_stage_v = std::is_same_v<
  decltype(std::declval<T &>().execute(std::declval<typename T::Input &&>())),
  typename T::Output>;

template <class A, class B>
class Pipeline final {
private:
  A _a;
  B _b;

  using This = Pipeline<A, B>;

public:
  using Input  = typename A::Input;
  using Output = typename B::Output;

  Pipeline(A &&a, B &&b) : _a(std::move(a)), _b(std::move(b)) {
  }

  Pipeline(const Pipeline &) = delete;
  Pipeline(Pipeline &&)      = default;

  static_assert(
    std::is_convertible_v<typename A::Output, typename B::Input>,
    "first pipeline stage's output is required "
    "to be compatible to the second stage's input");

  auto execute(Input &&input) noexcept -> Output {
    return _b.execute(_a.execute(std::move(input)));
  }

  auto has_errors() const noexcept -> bool {
    return _b.has_errors() || _b.has_errors();
  }

  auto errors() const noexcept -> const std::vector<std::unique_ptr<Error>> {
    // todo
    return _a.errors();
  }

  template <class Nxt>
  auto operator>>(Nxt &&nxt) noexcept
    -> std::enable_if_t<is_stage_v<Nxt>, Pipeline<This, Nxt>> {
    return Pipeline<This, Nxt>(std::move(*this), std::move(nxt));
  }
};

template <class D, class I = Module, class O = Module, class Nt = void>
class Stage {
private:
  std::vector<std::unique_ptr<Error>> mutable _errors{};

protected:
  auto report(std::string &&msg) const -> Error & {
    _errors.emplace_back(msg);
    return *_errors.back().get();
  }

  template <class E, class... Args>
  auto report(Args &&...args) const
    -> std::enable_if_t<std::is_base_of_v<Error, E>, E> & {
    _errors.push_back(
      std::unique_ptr<Error>(new E{std::forward<Args>(args)...}));
    return *dynamic_cast<E *>(_errors.back().get());
  }

  virtual auto handle(st::Node &, st::Comment &) -> Nt                   = 0;
  virtual auto handle(st::Node &, st::ModuleMain &) -> Nt                = 0;
  virtual auto handle(st::Node &, st::ModuleDeclaration &) -> Nt         = 0;
  virtual auto handle(st::Node &, st::ModuleImport &) -> Nt              = 0;
  virtual auto handle(st::Node &, st::DeclarationFunction &) -> Nt       = 0;
  virtual auto handle(st::Node &, st::DeclarationEnum &) -> Nt           = 0;
  virtual auto handle(st::Node &, st::DeclarationObject &) -> Nt         = 0;
  virtual auto handle(st::Node &, st::DeclarationDynamicLibrary &) -> Nt = 0;
  virtual auto handle(st::Node &, st::DeclarationMacro &) -> Nt          = 0;
  virtual auto handle(st::Node &, st::StatementEmpty &) -> Nt            = 0;
  virtual auto handle(st::Node &, st::StatementExpression &) -> Nt       = 0;
  virtual auto handle(st::Node &, st::StatementBlock &) -> Nt            = 0;
  virtual auto handle(st::Node &, st::StatementCircuit &) -> Nt          = 0;
  virtual auto handle(st::Node &, st::StatementVariable &) -> Nt         = 0;
  virtual auto handle(st::Node &, st::StatementConstant &) -> Nt         = 0;
  virtual auto handle(st::Node &, st::StatementReturn &) -> Nt           = 0;
  virtual auto handle(st::Node &, st::StatementSwitch &) -> Nt           = 0;
  virtual auto handle(st::Node &, st::StatementIterationControl &) -> Nt = 0;
  virtual auto handle(st::Node &, st::StatementIf &) -> Nt               = 0;
  virtual auto handle(st::Node &, st::StatementWhile &) -> Nt            = 0;
  virtual auto handle(st::Node &, st::StatementLoop &) -> Nt             = 0;
  virtual auto handle(st::Node &, st::StatementFor &) -> Nt              = 0;
  virtual auto handle(st::Node &, st::StatementForeach &) -> Nt          = 0;
  virtual auto handle(st::Node &, st::StatementMatch &) -> Nt            = 0;
  virtual auto handle(st::Node &, st::ExpressionIdentifier &) -> Nt      = 0;
  virtual auto handle(st::Node &, st::ExpressionVoid &) -> Nt            = 0;
  virtual auto handle(st::Node &, st::ExpressionContinuation &) -> Nt    = 0;
  virtual auto handle(st::Node &, st::ExpressionBool &) -> Nt            = 0;
  virtual auto handle(st::Node &, st::ExpressionNat &) -> Nt             = 0;
  virtual auto handle(st::Node &, st::ExpressionInt &) -> Nt             = 0;
  virtual auto handle(st::Node &, st::ExpressionReal &) -> Nt            = 0;
  virtual auto handle(st::Node &, st::ExpressionRealKeyword &) -> Nt     = 0;
  virtual auto handle(st::Node &, st::ExpressionChar &) -> Nt            = 0;
  virtual auto handle(st::Node &, st::ExpressionString &) -> Nt          = 0;
  virtual auto handle(st::Node &, st::ExpressionTuple &) -> Nt           = 0;
  virtual auto handle(st::Node &, st::ExpressionUnaryOp &) -> Nt         = 0;
  virtual auto handle(st::Node &, st::ExpressionBinaryOp &) -> Nt        = 0;
  virtual auto handle(st::Node &, st::ExpressionRange &) -> Nt           = 0;
  virtual auto handle(st::Node &, st::ExpressionVector &) -> Nt          = 0;
  virtual auto handle(st::Node &, st::ExpressionArray &) -> Nt           = 0;
  virtual auto handle(st::Node &, st::ExpressionDictionary &) -> Nt      = 0;
  virtual auto handle(st::Node &, st::ExpressionAssignment &) -> Nt      = 0;
  virtual auto handle(st::Node &, st::ExpressionCall &) -> Nt            = 0;
  virtual auto handle(st::Node &, st::ExpressionLambda &) -> Nt          = 0;

  auto handle_node(st::Node &node) -> Nt {
    return std::visit(
      [this, &node](auto &&data) { this->handle(node, data); }, node.data);
  }

public:
  using Input  = I;
  using Output = O;

  Stage()              = default;
  Stage(const Stage &) = delete;
  Stage(Stage &&)      = default;

  virtual ~Stage() {
  }

  virtual auto execute(I &&input) noexcept -> O = 0;

  virtual auto has_errors() const noexcept -> bool final {
    return !_errors.empty();
  }

  virtual auto errors() const noexcept
    -> const std::vector<std::unique_ptr<Error>> & {
    return _errors;
  }

  template <class Nxt>
  auto operator>>(Nxt &&nxt) noexcept
    -> std::enable_if_t<is_stage_v<Nxt>, Pipeline<D, Nxt>> {
    return Pipeline<D, Nxt>{
      std::move(*dynamic_cast<D *>(this)),
      std::move(nxt),
    };
  }
};

template <class D, class I, class O>
class NonSyntaxTreeStage : public Stage<D, I, O> {
private:
  auto handle(st::Node &, st::Comment &) -> void final{};
  auto handle(st::Node &, st::ModuleMain &) -> void final{};
  auto handle(st::Node &, st::ModuleDeclaration &) -> void final{};
  auto handle(st::Node &, st::ModuleImport &) -> void final{};
  auto handle(st::Node &, st::DeclarationFunction &) -> void final{};
  auto handle(st::Node &, st::DeclarationEnum &) -> void final{};
  auto handle(st::Node &, st::DeclarationObject &) -> void final{};
  auto handle(st::Node &, st::DeclarationDynamicLibrary &) -> void final{};
  auto handle(st::Node &, st::DeclarationMacro &) -> void final{};
  auto handle(st::Node &, st::StatementEmpty &) -> void final{};
  auto handle(st::Node &, st::StatementExpression &) -> void final{};
  auto handle(st::Node &, st::StatementBlock &) -> void final{};
  auto handle(st::Node &, st::StatementCircuit &) -> void final{};
  auto handle(st::Node &, st::StatementVariable &) -> void final{};
  auto handle(st::Node &, st::StatementConstant &) -> void final{};
  auto handle(st::Node &, st::StatementReturn &) -> void final{};
  auto handle(st::Node &, st::StatementSwitch &) -> void final{};
  auto handle(st::Node &, st::StatementIterationControl &) -> void final{};
  auto handle(st::Node &, st::StatementIf &) -> void final{};
  auto handle(st::Node &, st::StatementWhile &) -> void final{};
  auto handle(st::Node &, st::StatementLoop &) -> void final{};
  auto handle(st::Node &, st::StatementFor &) -> void final{};
  auto handle(st::Node &, st::StatementForeach &) -> void final{};
  auto handle(st::Node &, st::StatementMatch &) -> void final{};
  auto handle(st::Node &, st::ExpressionIdentifier &) -> void final{};
  auto handle(st::Node &, st::ExpressionVoid &) -> void final{};
  auto handle(st::Node &, st::ExpressionContinuation &) -> void final{};
  auto handle(st::Node &, st::ExpressionBool &) -> void final{};
  auto handle(st::Node &, st::ExpressionNat &) -> void final{};
  auto handle(st::Node &, st::ExpressionInt &) -> void final{};
  auto handle(st::Node &, st::ExpressionReal &) -> void final{};
  auto handle(st::Node &, st::ExpressionRealKeyword &) -> void final{};
  auto handle(st::Node &, st::ExpressionChar &) -> void final{};
  auto handle(st::Node &, st::ExpressionString &) -> void final{};
  auto handle(st::Node &, st::ExpressionTuple &) -> void final{};
  auto handle(st::Node &, st::ExpressionUnaryOp &) -> void final{};
  auto handle(st::Node &, st::ExpressionBinaryOp &) -> void final{};
  auto handle(st::Node &, st::ExpressionRange &) -> void final{};
  auto handle(st::Node &, st::ExpressionVector &) -> void final{};
  auto handle(st::Node &, st::ExpressionArray &) -> void final{};
  auto handle(st::Node &, st::ExpressionDictionary &) -> void final{};
  auto handle(st::Node &, st::ExpressionAssignment &) -> void final{};
  auto handle(st::Node &, st::ExpressionCall &) -> void final{};
  auto handle(st::Node &, st::ExpressionLambda &) -> void final{};
};

} // namespace silk
