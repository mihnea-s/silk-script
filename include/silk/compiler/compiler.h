#pragma once

#include <cstdint>
#include <map>
#include <vector>

#include <vm/program.h>

#include "../common/ast.h"
#include "../common/error.h"
#include "../common/token.h"

class Compiler : ASTVisitor<void>, public ErrorReporter {
  private:
  Program _program;

  // for interning constants
  std::unordered_map<std::int32_t, std::uint8_t> _integers = {};
  std::unordered_map<double, std::uint8_t>       _reals    = {};
  std::unordered_map<std::string, std::uint8_t>  _strings  = {};

  // chunk functions
  auto current_chunk() -> Chunk*;

  // bytecode functions
  auto emit(std::uint8_t) -> void;
  auto cnst(Value) -> std::uint8_t;

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
  auto evaluate(const Identifier&) -> void final;
  auto evaluate(const Grouping&) -> void final;
  auto evaluate(const Call&) -> void final;
  auto evaluate(const Get&) -> void final;

  auto execute(const Empty&) -> void final;
  auto execute(const Package&) -> void final;
  auto execute(const Variable&) -> void final;
  auto execute(const Function&) -> void final;
  auto execute(const Struct&) -> void final;
  auto execute(const Loop&) -> void final;
  auto execute(const Conditional&) -> void final;
  auto execute(const Block&) -> void final;
  auto execute(const Interrupt&) -> void final;
  auto execute(const ExprStmt&) -> void final;

  public:
  // compile entrypoint
  auto compile(const AST&) noexcept -> void;
  auto write_to_file(std::string_view) noexcept -> void;

  ~Compiler() {
    free_program(&_program);
  }
};
