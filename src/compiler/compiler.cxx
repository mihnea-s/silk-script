#include "silk/common/ast.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <vm/chunk.h>
#include <vm/constants.h>
#include <vm/file_exec.h>
#include <vm/mem.h>
#include <vm/object.h>
#include <vm/opcode.h>
#include <vm/value.h>

#include <silk/common/error.h>
#include <silk/common/token.h>
#include <silk/compiler/compiler.h>

auto Compiler::current_chunk() -> Chunk* {
  if (_program.len == 0) {
    _program.len  = 1;
    _program.cnks = (Chunk*)memory(NULL, 0x0, sizeof(Chunk));
    init_chunk(_program.cnks);
  }

  return _program.cnks + (_program.len - 1);
}

auto Compiler::cnst(Value value) -> std::uint8_t {
  auto cnk = current_chunk();
  auto id  = cnk->constants.len;
  write_ins(cnk, VM_VAL);
  write_ins(cnk, id);
  write_constant(&cnk->constants, value);
  return id;
}

auto Compiler::emit(std::uint8_t byte) -> void {
  write_ins(current_chunk(), byte);
}

auto Compiler::evaluate(const Unary& node) -> void {
  visit_node(node.operand);
  switch (node.operation) {
    case TokenType::sym_bang: return emit(VM_NOT);
    case TokenType::sym_minus: return emit(VM_NEG);
    default:
      throw report_error(node.location, "invalid operand to unary operation");
  }
}

auto Compiler::evaluate(const Binary& node) -> void {
  visit_node(node.right);
  visit_node(node.left);

  switch (node.operation) {
    case TokenType::sym_plus: return emit(VM_ADD);

    default:
      throw report_error(node.location, "invalid operand to binary operation");
  }
}

auto Compiler::evaluate(const IntLiteral& node) -> void {
  if (_integers.find(node.value) != _integers.end()) {
    emit(_integers[node.value]);
    return;
  }

  Value value = {
    .type       = T_INT,
    .as.integer = node.value,
  };

  auto constant_id      = cnst(value);
  _integers[node.value] = constant_id;
}

auto Compiler::evaluate(const RealLiteral& node) -> void {
  if (_reals.find(node.value) != _reals.end()) {
    emit(_reals[node.value]);
    return;
  }

  Value value = {
    .type    = T_REAL,
    .as.real = node.value,
  };

  auto constant_id   = cnst(value);
  _reals[node.value] = constant_id;
}

auto Compiler::evaluate(const BoolLiteral& node) -> void {
  emit(node.value ? VM_TRU : VM_FAL);
}

auto Compiler::evaluate(const StringLiteral& node) -> void {
  if (_strings.find(node.value) != _strings.end()) {
    emit(_strings[node.value]);
    return;
  }

  auto c_str = (char*)memory(NULL, 0x0, sizeof(char) * node.value.size() + 1);
  memcpy(c_str, node.value.data(), node.value.size());
  c_str[node.value.size()] = '\0';

  Value value = {
    .type      = T_STR,
    .as.string = c_str,
  };

  auto constant_id     = cnst(value);
  _strings[node.value] = constant_id;
}

auto Compiler::evaluate(const Vid&) -> void {
  emit(VM_VID);
}

auto Compiler::evaluate(const Constant& node) -> void {
  switch (node.which) {
    case Constant::PI: return emit(VM_PI);
    case Constant::TAU: return emit(VM_TAU);
    case Constant::EULER: return emit(VM_EUL);
  }
}

auto Compiler::evaluate(const Lambda& group) -> void {
}

auto Compiler::evaluate(const Assignment& assignment) -> void {
}

auto Compiler::evaluate(const Identifier& id) -> void {
}

auto Compiler::evaluate(const Grouping& group) -> void {
}

auto Compiler::evaluate(const Call& call) -> void {
}

auto Compiler::evaluate(const Get& get) -> void {
}

auto Compiler::execute(const Empty&) -> void {
  emit(VM_NOP);
}

auto Compiler::execute(const Package& pkg) -> void {
}

auto Compiler::execute(const Variable& var) -> void {
}

auto Compiler::execute(const Function& fct) -> void {
}

auto Compiler::execute(const Struct&) -> void {
}

auto Compiler::execute(const Loop& l) -> void {
}

auto Compiler::execute(const Conditional& cond) -> void {
}

auto Compiler::execute(const Block& blk) -> void {
}

auto Compiler::execute(const Interrupt& r) -> void {
}

auto Compiler::execute(const ExprStmt& e) -> void {
}

// public compile function

auto Compiler::compile(const AST& ast) noexcept -> void {
  init_program(&_program);

  try {
    for (const auto& node : ast.program) {
      visit_node(node);
    }
  } catch (...) {}
}

auto Compiler::write_to_file(std::string_view file) noexcept -> void {
  const char* err = nullptr;
  write_file(file.data(), &_program, &err);
  if (err) report_error({0, 0}, err);
}
