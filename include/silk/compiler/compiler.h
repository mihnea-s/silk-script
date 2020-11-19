#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <stack>
#include <string_view>
#include <vector>

#include <moth/program.h>
#include <moth/value.h>
#include <moth/vm.h>

#include <silk/lexer/token.h>
#include <silk/parser/ast.h>
#include <silk/util/error.h>

class Compiler : ASTHandler<void, void>, public ErrorReporter {
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

  auto evaluate(Identifier&) -> void final;
  auto evaluate(Unary&) -> void final;
  auto evaluate(Binary&) -> void final;
  auto evaluate(BoolLiteral&) -> void final;
  auto evaluate(IntLiteral&) -> void final;
  auto evaluate(RealLiteral&) -> void final;
  auto evaluate(StringLiteral&) -> void final;
  auto evaluate(ArrayLiteral&) -> void final;
  auto evaluate(Constant&) -> void final;
  auto evaluate(Lambda&) -> void final;
  auto evaluate(Assignment&) -> void final;
  auto evaluate(Call&) -> void final;
  auto evaluate(Access&) -> void final;
  auto evaluate(ConstExpr&) -> void final;

  auto execute(Empty&) -> void final;
  auto execute(Package&) -> void final;
  auto execute(ExprStmt&) -> void final;
  auto execute(Block&) -> void final;
  auto execute(Conditional&) -> void final;
  auto execute(Loop&) -> void final;
  auto execute(Foreach&) -> void final;
  auto execute(Match&) -> void final;
  auto execute(MatchCase&) -> void final;
  auto execute(ControlFlow&) -> void final;
  auto execute(Return&) -> void final;
  auto execute(Variable&) -> void final;
  auto execute(Function&) -> void final;
  auto execute(Struct&) -> void final;

  public:
  ~Compiler() {
    free_program();
  }

  // compile entrypoint
  auto compile(const AST&) noexcept -> void;
  auto write_to_file(std::string_view) noexcept -> void;
  auto run_in_vm(VM*) noexcept -> VMStatus;
  auto free_program() noexcept -> void;
};
