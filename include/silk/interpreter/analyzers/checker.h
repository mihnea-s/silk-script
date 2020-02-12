#pragma once

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "../../common/error.h"
#include "../ast/ast.h"
#include "../ast/expr.h"
#include "../ast/stmt.h"

enum class SilkType {
  DYNAMIC,
  INTEGER,
  REAL,
  STRING,
  BOOLEAN,
  CALLABLE,
  INSTANCE,
};

struct Checker : Expr::Visitor<SilkType>, Stmt::Visitor<std::nullptr_t> {
  private:
  std::vector<ParsingError>       _errors;
  std::map<std::string, SilkType> _variables;

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
  template <class... Args>
  inline auto throw_error(std::string_view frmt, Args... args) -> void {
    throw ParsingError {
      Severity::error,
      fmt::format(frmt, std::forward<Args>(args)...),
      {0, 0}, // TODO
    };
  }

  auto evaluate(const Expr::Unary& expr) -> SilkType override;
  auto evaluate(const Expr::Binary& expr) -> SilkType override;

  auto evaluate(const Expr::IntLiteral& integer) -> SilkType override;
  auto evaluate(const Expr::DoubleLiteral& dbl) -> SilkType override;
  auto evaluate(const Expr::BoolLiteral& boolean) -> SilkType override;
  auto evaluate(const Expr::StringLiteral& str) -> SilkType override;
  auto evaluate(const Expr::Vid&) -> SilkType override;
  auto evaluate(const Expr::Lambda& group) -> SilkType override;

  auto evaluate(const Expr::Assignment& assignment) -> SilkType override;
  auto evaluate(const Expr::Identifier& id) -> SilkType override;
  auto evaluate(const Expr::Grouping& group) -> SilkType override;

  auto evaluate(const Expr::Call& call) -> SilkType override;
  auto evaluate(const Expr::Get& get) -> SilkType override;

  auto execute(const Stmt::Empty&) -> std::nullptr_t override;

  auto execute(const Stmt::Main&) -> std::nullptr_t override;
  auto execute(const Stmt::Package& pkg) -> std::nullptr_t override;
  auto execute(const Stmt::Import& imprt) -> std::nullptr_t override;

  auto execute(const Stmt::Variable& var) -> std::nullptr_t override;
  auto execute(const Stmt::Function& fct) -> std::nullptr_t override;
  auto execute(const Stmt::Struct&) -> std::nullptr_t override;

  auto execute(const Stmt::Loop& l) -> std::nullptr_t override;
  auto execute(const Stmt::Interrupt& i) -> std::nullptr_t override;
  auto execute(const Stmt::Conditional& cond) -> std::nullptr_t override;
  auto execute(const Stmt::Block& blk) -> std::nullptr_t override;
  auto execute(const Stmt::ExprStmt& e) -> std::nullptr_t override;

  public:
  auto has_error() const -> bool;
  auto clear_errors() -> void;
  auto errors() -> std::vector<ParsingError>&;

  auto check(AST& ast) -> void;
};
