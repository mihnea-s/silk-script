#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <stack>
#include <string_view>
#include <vector>

#include <vm/program.h>
#include <vm/value.h>

#include "../common/ast.h"
#include "../common/error.h"
#include "../common/token.h"

class Compiler : ASTVisitor<void>, public ErrorReporter {
  private:
  typedef struct {
    std::string_view              name;
    std::vector<std::string_view> imports;
  } Pkginfo;

  typedef struct {
    std::string_view name;
    int              depth;
    bool             is_const;
    int              slot = -1;
  } Varinfo;

  typedef struct {
    int                  depth = -1;
    std::vector<Varinfo> decls = {};
  } Locals;

  typedef struct {
    using Buffer = std::vector<uint8_t>;
    Locals locals;
    Buffer buffer;
  } Target;

  Program            _program;
  Locals             _main_locals = {};
  Pkginfo            _pkginfo     = {};
  std::stack<Target> _targets     = {};

  // for interning read only data
  std::unordered_map<std::int32_t, std::uint32_t>     _integers = {};
  std::unordered_map<double, std::uint32_t>           _reals    = {};
  std::unordered_map<std::string_view, std::uint32_t> _strings  = {};
  std::unordered_map<std::string_view, std::uint32_t> _symbols  = {};

  // bytecode functions
  auto emit(std::uint8_t) -> void;
  auto argx(std::uint32_t, std::size_t) -> void;
  auto argx_op(std::uint8_t, std::uint32_t) -> void;

  auto get_offset() -> std::uint32_t;
  auto get_buffer() -> std::uint8_t*;

  auto push_scope() -> void;
  auto pop_scope() -> void;

  auto define_stack_var(std::string_view, bool) -> bool;

  auto get_stack_var(std::string_view) -> const Varinfo*;
  auto load_stack_var(std::uint16_t) -> void;
  auto store_stack_var(std::uint16_t) -> void;

  auto get_upvalue(std::string_view) -> const Varinfo*;
  auto load_upvalue(int, int) -> void;
  auto store_upvalue(int, int) -> void;

  auto encode_rodata(Value) -> std::uint32_t;
  auto load_rodata(std::uint32_t) -> void;

  auto encode_symbol(Symbol) -> std::uint32_t;
  auto encode_symbol(std::string_view) -> std::uint32_t;
  auto load_symbol(std::uint32_t) -> void;
  auto define_symbol(std::uint32_t) -> void;
  auto assign_symbol(std::uint32_t) -> void;

  auto jmp_insert(std::uint8_t) -> std::uint32_t;
  auto jmp_finish(std::uint32_t) -> void;

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
  ~Compiler() {
    free_program();
  }

  // compile entrypoint
  auto compile(const AST&) noexcept -> void;
  auto write_to_file(std::string_view) noexcept -> void;
  auto free_program() noexcept -> void;
};
