#pragma once

#include <cmath>
#include <cstdint>
#include <memory>
#include <optional>
#include <type_traits>

#include <silk/language/package.h>
#include <silk/language/syntax_tree.h>
#include <silk/pipeline/stage.h>

namespace silk {

class Optimizer final : public Stage<Optimizer, Module, Module> {
private:
  auto nat_value(st::Node &) const -> std::optional<std::uint64_t>;
  auto int_value(st::Node &) const -> std::optional<std::int64_t>;
  auto real_value(st::Node &) const -> std::optional<double>;

  auto is_const_expr(std::unique_ptr<st::Node>& ) const -> bool;
  
  template<class T, class A, class B>
  auto make_const_bin_expr(st::ExpressionBinaryOp::Kind kind, A a, B b) -> T {
    using OPv = decltype(T::value);

    switch (kind) {
      case st::ExpressionBinaryOp::ADD:
        return T { static_cast<OPv>(a + b) };

      case st::ExpressionBinaryOp::SUB:
        return T { static_cast<OPv>(a - b) };

      case st::ExpressionBinaryOp::MUL:
        return T { static_cast<OPv>(a * b) };

      case st::ExpressionBinaryOp::POW:
        return T { static_cast<OPv>(std::pow(a, b)) };

      case st::ExpressionBinaryOp::DIV:
        return T { static_cast<OPv>(a / b) };

      case st::ExpressionBinaryOp::RDIV:
        return T { static_cast<OPv>(std::floor(a / b)) };

      case st::ExpressionBinaryOp::MOD:
        if constexpr (std::is_integral_v<A> && std::is_integral_v<B>)
          return T { static_cast<OPv>(a % b) };
        else
          return T { static_cast<OPv>(std::fmod(a, b)) };

      default:
        throw report("invalid binary operation");
    }
  }

  auto handle(st::Node &, st::Comment &) -> void override;
  auto handle(st::Node &, st::ModuleMain &) -> void override;
  auto handle(st::Node &, st::ModuleDeclaration &) -> void override;
  auto handle(st::Node &, st::ModuleImport &) -> void override;
  auto handle(st::Node &, st::DeclarationFunction &) -> void override;
  auto handle(st::Node &, st::DeclarationEnum &) -> void override;
  auto handle(st::Node &, st::DeclarationObject &) -> void override;
  auto handle(st::Node &, st::DeclarationExternLibrary &) -> void override;
  auto handle(st::Node &, st::DeclarationExternFunction &) -> void override;
  auto handle(st::Node &, st::DeclarationMacro &) -> void override;
  auto handle(st::Node &, st::StatementEmpty &) -> void override;
  auto handle(st::Node &, st::StatementExpression &) -> void override;
  auto handle(st::Node &, st::StatementBlock &) -> void override;
  auto handle(st::Node &, st::StatementCircuit &) -> void override;
  auto handle(st::Node &, st::StatementVariable &) -> void override;
  auto handle(st::Node &, st::StatementConstant &) -> void override;
  auto handle(st::Node &, st::StatementReturn &) -> void override;
  auto handle(st::Node &, st::StatementSwitch &) -> void override;
  auto handle(st::Node &, st::StatementIterationControl &) -> void override;
  auto handle(st::Node &, st::StatementIf &) -> void override;
  auto handle(st::Node &, st::StatementWhile &) -> void override;
  auto handle(st::Node &, st::StatementLoop &) -> void override;
  auto handle(st::Node &, st::StatementFor &) -> void override;
  auto handle(st::Node &, st::StatementForeach &) -> void override;
  auto handle(st::Node &, st::StatementMatch &) -> void override;
  auto handle(st::Node &, st::ExpressionIdentifier &) -> void override;
  auto handle(st::Node &, st::ExpressionVoid &) -> void override;
  auto handle(st::Node &, st::ExpressionContinuation &) -> void override;
  auto handle(st::Node &, st::ExpressionBool &) -> void override;
  auto handle(st::Node &, st::ExpressionNat &) -> void override;
  auto handle(st::Node &, st::ExpressionInt &) -> void override;
  auto handle(st::Node &, st::ExpressionReal &) -> void override;
  auto handle(st::Node &, st::ExpressionRealKeyword &) -> void override;
  auto handle(st::Node &, st::ExpressionChar &) -> void override;
  auto handle(st::Node &, st::ExpressionString &) -> void override;
  auto handle(st::Node &, st::ExpressionTuple &) -> void override;
  auto handle(st::Node &, st::ExpressionUnaryOp &) -> void override;
  auto handle(st::Node &, st::ExpressionBinaryOp &) -> void override;
  auto handle(st::Node &, st::ExpressionRange &) -> void override;
  auto handle(st::Node &, st::ExpressionVector &) -> void override;
  auto handle(st::Node &, st::ExpressionArray &) -> void override;
  auto handle(st::Node &, st::ExpressionDictionary &) -> void override;
  auto handle(st::Node &, st::ExpressionAssignment &) -> void override;
  auto handle(st::Node &, st::ExpressionCall &) -> void override;
  auto handle(st::Node &, st::ExpressionLambda &) -> void override;

public:
  Optimizer() {
  }

  ~Optimizer() {
  }

  Optimizer(const Optimizer &) = delete;
  Optimizer(Optimizer &&)      = default;

  auto execute(Module &&) noexcept -> Module override;
};

} // namespace silk
