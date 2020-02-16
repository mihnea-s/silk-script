#pragma once

#include <cstdint>
#include <map>
#include <string_view>
#include <vector>

#include <vm/object.h>
#include <vm/program.h>

#include "../common/ast.h"
#include "../common/error.h"
#include "../common/token.h"

class Compiler : ASTVisitor<void>, public ErrorReporter {
  private:
  Program _program;

  struct {
    std::string_view              name;
    std::vector<std::string_view> imports;
  } _pkginfo;

  typedef struct {
    std::string_view name;
    int              depth;
    bool             is_const;
  } _varinfo;

  struct {
    bool                 is_function;
    std::vector<uint8_t> buffer;
  } _target;

  // for interning read only data
  std::unordered_map<std::int32_t, std::uint32_t>     _integers = {};
  std::unordered_map<double, std::uint32_t>           _reals    = {};
  std::unordered_map<std::string_view, std::uint32_t> _strings  = {};
  std::unordered_map<std::string_view, std::uint32_t> _symbols  = {};

  // for keeping track of variables
  int                   _depth     = -1;
  std::vector<_varinfo> _variables = {};

  // bytecode functions
  auto emit(std::uint8_t) -> void;
  auto argx(std::uint32_t, std::size_t) -> void;
  auto argx_op(std::uint8_t, std::uint32_t) -> void;

  auto load_var(std::uint8_t) -> void;
  auto load_val(std::uint32_t) -> void;
  auto load_sym(std::uint32_t) -> void;
  auto define_sym(std::uint32_t) -> void;
  auto assign_sym(std::uint32_t) -> void;

  auto jmp_insert(std::uint8_t) -> std::uint32_t;
  auto jmp_finish(std::uint32_t) -> void;

  auto push_scope() -> void;
  auto pop_scope() -> void;
  auto to_scope(std::string_view, bool) -> bool;
  auto from_scope(std::string_view) const -> const std::pair<_varinfo, int>;

  auto encode_rodata(Value) -> std::uint32_t;
  auto encode_symbol(Symbol) -> std::uint32_t;
  auto encode_symbol_from_str(std::string_view) -> std::uint32_t;

  auto evaluate(const Unary&) -> void final;
  auto evaluate(const Binary&) -> void final;
  auto evaluate(const IntLiteral&) -> void final;
  auto evaluate(const RealLiteral&) -> void final;
  auto evaluate(const BoolLiteral&) -> void final;
  auto evaluate(const StringLiteral& str) -> void final;
  auto evaluate(const Vid&) -> void final;
  auto evaluate(const Constant&) -> void final;
  auto evaluate(const Lambda&) -> void final;
  auto evaluate(const Assignment&) -> void final;
  auto evaluate(const IdentifierRef&) -> void final;
  auto evaluate(const IdentifierVal&) -> void final;
  auto evaluate(const Grouping&) -> void final;
  auto evaluate(const Call&) -> void final;
  auto evaluate(const Access&) -> void final;
  auto evaluate(const ConstExpr&) -> void final;

  auto execute(const Empty&) -> void final;
  auto execute(const Package&) -> void final;
  auto execute(const Variable&) -> void final;
  auto execute(const Function&) -> void final;
  auto execute(const Struct&) -> void final;
  auto execute(const Loop&) -> void final;
  auto execute(const Conditional&) -> void final;
  auto execute(const Match&) -> void final;
  auto execute(const MatchCase&) -> void final;
  auto execute(const Block&) -> void final;
  auto execute(const Interrupt&) -> void final;
  auto execute(const ExprStmt&) -> void final;
  auto execute(const Return&) -> void final;

  public:
  // compile entrypoint
  auto compile(const AST&) noexcept -> void;
  auto write_to_file(std::string_view) noexcept -> void;

  ~Compiler() {
    free_program(&_program);
  }
};
