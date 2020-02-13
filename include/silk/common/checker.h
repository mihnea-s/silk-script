#pragma once

#include <map>

#include "ast.h"
#include "error.h"

enum class SilkType {
  DYNAMIC,
  INTEGER,
  REAL,
  STRING,
  BOOLEAN,
  CALLABLE,
  INSTANCE,
  NONE,
};

class Checker : ASTVisitor<SilkType>, public ErrorReporter {
  private:
  std::unordered_map<std::string, SilkType> _variables;

  auto declare_var(const std::string& name, SilkType type) -> void;
  auto variable_type(const std::string& name) -> SilkType;

  template <class... Args>
  constexpr auto of_type_or_dyn(SilkType type, Args... others) -> bool {
    if (type == SilkType::DYNAMIC) return true;
    return of_type(type, others...);
  }

  template <class... Args>
  constexpr auto of_type(SilkType type, Args... others) -> bool {
    return ((type == others) || ...);
  }

  constexpr auto type_from_str(const std::string& type_str) -> SilkType {
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

  // error helper

  SilkType evaluate(const Unary&) final;
  SilkType evaluate(const Binary&) final;
  SilkType evaluate(const IntLiteral&) final;
  SilkType evaluate(const RealLiteral&) final;
  SilkType evaluate(const BoolLiteral&) final;
  SilkType evaluate(const StringLiteral&) final;
  SilkType evaluate(const Vid&) final;
  SilkType evaluate(const Constant&) final;
  SilkType evaluate(const Lambda&) final;
  SilkType evaluate(const Identifier&) final;
  SilkType evaluate(const Assignment&) final;
  SilkType evaluate(const Grouping&) final;
  SilkType evaluate(const Call&) final;
  SilkType evaluate(const Get&) final;

  SilkType execute(const Empty&) final;
  SilkType execute(const Package&) final;
  SilkType execute(const ExprStmt&) final;
  SilkType execute(const Variable&) final;
  SilkType execute(const Function&) final;
  SilkType execute(const Struct&) final;
  SilkType execute(const Loop&) final;
  SilkType execute(const Conditional&) final;
  SilkType execute(const Block&) final;
  SilkType execute(const Interrupt&) final;

  public:
  // check ast function
  auto check(const AST& ast) noexcept -> void;
};