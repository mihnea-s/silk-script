#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <stack>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <moth/program.h>
#include <moth/value.h>
#include <moth/vm.h>

#include <silk/parser/ast.h>
#include <silk/util/error.h>

class Compiler final : public ErrorReporter, public ASTHandler<void, void> {
private:
  struct VarInfo {
    int slot = -1;

    std::string_view name;

    int  depth;
    bool immut;
  };

  using Buffer = std::vector<uint8_t>;
  using Locals = std::vector<VarInfo>;

  struct Target {
    int    depth = -1;
    Locals locals;
    Buffer buffer;
  };

  Program _program;

  int    _main_depth  = -1;
  Locals _main_locals = {};

  std::string_view              _pkg_name    = {};
  std::vector<std::string_view> _pkg_imports = {};
  std::stack<Target>            _targets     = {};

  // Caching the id of read
  std::unordered_map<std::int32_t, std::uint32_t>     _integers = {};
  std::unordered_map<double, std::uint32_t>           _reals    = {};
  std::unordered_map<char32_t, std::uint32_t>         _chars    = {};
  std::unordered_map<std::string_view, std::uint32_t> _strings  = {};
  std::unordered_map<std::string_view, std::uint32_t> _symbols  = {};

  // bytecode functions
  auto emit(std::uint8_t) -> void;
  auto emit_arg(std::uint32_t, std::size_t) -> void;
  auto emit_op_arg(std::uint8_t, std::uint32_t) -> void;

  auto get_offset() -> std::uint32_t;
  auto get_buffer() -> std::uint8_t *;

  auto push_scope() -> void;
  auto pop_scope() -> void;

  auto define_stack_var(std::string_view, bool) -> bool;
  auto get_stack_var(std::string_view) -> const VarInfo *;
  auto load_stack_var(std::uint16_t) -> void;
  auto store_stack_var(std::uint16_t) -> void;

  auto get_upvalue(std::string_view) -> const VarInfo *;
  auto load_upvalue(int, int) -> void;
  auto store_upvalue(int, int) -> void;

  auto encode_rodata(Value) -> std::uint32_t;
  auto load_rodata(std::uint32_t) -> void;

  auto encode_symbol(std::string_view) -> std::uint32_t;
  auto load_symbol(std::uint32_t) -> void;
  auto define_symbol(std::uint32_t) -> void;
  auto assign_symbol(std::uint32_t) -> void;

  auto jmp_insert(std::uint8_t) -> std::uint32_t;
  auto jmp_finish(std::uint32_t) -> void;

  auto logical_or(ASTNode &, ASTNode &) -> void;
  auto logical_and(ASTNode &, ASTNode &) -> void;

  auto evaluate(ASTNode &, Identifier &) -> void override;
  auto evaluate(ASTNode &, Unary &) -> void override;
  auto evaluate(ASTNode &, Binary &) -> void override;
  auto evaluate(ASTNode &, KeyLiteral &) -> void override;
  auto evaluate(ASTNode &, BoolLiteral &) -> void override;
  auto evaluate(ASTNode &, IntLiteral &) -> void override;
  auto evaluate(ASTNode &, RealLiteral &) -> void override;
  auto evaluate(ASTNode &, CharLiteral &) -> void override;
  auto evaluate(ASTNode &, StringLiteral &) -> void override;
  auto evaluate(ASTNode &, ArrayLiteral &) -> void override;
  auto evaluate(ASTNode &, VectorLiteral &) -> void override;
  auto evaluate(ASTNode &, Lambda &) -> void override;
  auto evaluate(ASTNode &, Assignment &) -> void override;
  auto evaluate(ASTNode &, Call &) -> void override;
  auto evaluate(ASTNode &, Access &) -> void override;

  auto execute(Empty &) -> void override;
  auto execute(Package &) -> void override;
  auto execute(Variable &) -> void override;
  auto execute(ExprStmt &) -> void override;
  auto execute(Block &) -> void override;
  auto execute(Conditional &) -> void override;
  auto execute(Loop &) -> void override;
  auto execute(Foreach &) -> void override;
  auto execute(Match &) -> void override;
  auto execute(MatchCase &) -> void override;
  auto execute(ControlFlow &) -> void override;
  auto execute(Return &) -> void override;
  auto execute(Constant &) -> void override;
  auto execute(Function &) -> void override;
  auto execute(Enum &) -> void override;
  auto execute(Struct &) -> void override;
  auto execute(Main &) -> void override;

public:
  Compiler() {
  }

  ~Compiler() {
    free_program(&_program);
  }

  auto compile(AST &) noexcept -> void;

  /// Access the bytecode program produced by the compiler
  inline auto bytecode() noexcept -> Program & {
    return _program;
  }
};
