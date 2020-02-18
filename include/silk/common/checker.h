#pragma once

#include <map>

#include "ast.h"
#include "error.h"

// struct

struct CheckerExprResult {
  enum SilkType {
    DYNAMIC,
    INTEGER,
    REAL,
    STRING,
    BOOLEAN,
    CALLABLE,
    INSTANCE,
    NONE,
  };

  SilkType type;
  bool     assignable;
};

class Checker : ASTVisitor<CheckerExprResult>, public ErrorReporter {
  private:
  using SilkType = CheckerExprResult::SilkType;

  std::unordered_map<std::string, SilkType> _variables;

  auto declare_var(const std::string& name, SilkType type) -> void;
  auto variable_type(const std::string& name) -> SilkType;

  template <class... Args>
  constexpr auto of_type_or_dyn(SilkType type, Args... others) -> bool {
    if (type == SilkType::DYNAMIC) return true;
    return of_type(type, others...) || ((others == SilkType::DYNAMIC) || ...);
  }

  template <class... Args>
  constexpr auto of_type(SilkType type, Args... others) -> bool {
    return ((type == others) || ...);
  }

  auto type_from_str(const std::string& type_str) -> SilkType {
    if (type_str.empty() || type_str.compare("?") == 0)
      return SilkType::DYNAMIC;

    if (type_str.compare("int") == 0) return SilkType::INTEGER;
    if (type_str.compare("real") == 0) return SilkType::REAL;
    if (type_str.compare("bool") == 0) return SilkType::BOOLEAN;
    if (type_str.compare("str") == 0) return SilkType::STRING;
    if (type_str.compare("callable") == 0) return SilkType::DYNAMIC;

    return SilkType::INSTANCE;
  }

  constexpr auto str_from_type(const SilkType& type) -> const std::string_view {
    switch (type) {
      case SilkType::INTEGER: return "int";
      case SilkType::REAL: return "real";
      case SilkType::STRING: return "str";
      case SilkType::BOOLEAN: return "bool";
      case SilkType::CALLABLE: return "callable";
      default: return "dyn";
    }
  }

  CheckerExprResult stmt_result() const noexcept;

  CheckerExprResult evaluate(const Unary&) final;
  CheckerExprResult evaluate(const Binary&) final;
  CheckerExprResult evaluate(const IntLiteral&) final;
  CheckerExprResult evaluate(const RealLiteral&) final;
  CheckerExprResult evaluate(const BoolLiteral&) final;
  CheckerExprResult evaluate(const StringLiteral&) final;
  CheckerExprResult evaluate(const Vid&) final;
  CheckerExprResult evaluate(const Constant&) final;
  CheckerExprResult evaluate(const Lambda&) final;
  CheckerExprResult evaluate(const IdentifierVal&) final;
  CheckerExprResult evaluate(const IdentifierRef&) final;
  CheckerExprResult evaluate(const Assignment&) final;
  CheckerExprResult evaluate(const Grouping&) final;
  CheckerExprResult evaluate(const Call&) final;
  CheckerExprResult evaluate(const Access&) final;
  CheckerExprResult evaluate(const ConstExpr&) final;

  CheckerExprResult execute(const Empty&) final;
  CheckerExprResult execute(const Package&) final;
  CheckerExprResult execute(const ExprStmt&) final;
  CheckerExprResult execute(const Variable&) final;
  CheckerExprResult execute(const Function&) final;
  CheckerExprResult execute(const Struct&) final;
  CheckerExprResult execute(const Loop&) final;
  CheckerExprResult execute(const Conditional&) final;
  CheckerExprResult execute(const Match&) final;
  CheckerExprResult execute(const MatchCase&) final;
  CheckerExprResult execute(const Block&) final;
  CheckerExprResult execute(const Interrupt&) final;
  CheckerExprResult execute(const Return&) final;

  public:
  // check ast function
  auto check(const AST& ast) noexcept -> void;
};