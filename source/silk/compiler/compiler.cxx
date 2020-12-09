#include <silk/compiler/compiler.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>

#include <moth/file.h>
#include <moth/macros.h>
#include <moth/mem.h>
#include <moth/object.h>
#include <moth/opcode.h>
#include <moth/program.h>
#include <moth/rodata.h>
#include <moth/symtable.h>
#include <moth/value.h>
#include <moth/vm.h>

#include <silk/parser/ast.h>
#include <silk/parser/token.h>
#include <silk/util/error.h>

auto Compiler::emit(std::uint8_t byte) -> void {
  if (_targets.empty()) {
    write_byte(&_program, byte);
  } else {
    _targets.top().buffer.push_back(byte);
  }
}

auto Compiler::emit_arg(std::uint32_t arg, std::size_t size) -> void {
  if constexpr (IS_BIG_ENDIAN) arg = SWAP_BYTES(arg);

  for (int i = size - 1; i >= 0; i--) {
    emit((arg >> (8 * i)) & 0xff);
  }
}

auto Compiler::emit_op_arg(std::uint8_t base_op, std::uint32_t id) -> void {
  constexpr auto BYTE_MAX = std::numeric_limits<std::uint8_t>::max();

  if (id <= BYTE_MAX) {
    emit(base_op + 0);
    emit(id & 0xff);
  } else if (id <= BYTE_MAX * 2) {
    emit(base_op + 1);
    emit_arg(id, 2);
  } else if (id <= BYTE_MAX * 3) {
    emit(base_op + 2);
    emit_arg(id, 3);
  } else {
    emit(base_op + 3);
    emit_arg(id, 4);
  }
}

auto Compiler::get_offset() -> std::uint32_t {
  if (_targets.empty()) {
    return _program.len;
  } else {
    return _targets.top().buffer.size();
  }
}

auto Compiler::get_buffer() -> std::uint8_t * {
  if (_targets.empty()) {
    return _program.bytes;
  } else {
    return _targets.top().buffer.data();
  }
}

auto Compiler::push_scope() -> void {
  if (_targets.empty()) {
    _main_depth++;
  } else {
    _targets.top().depth++;
  }
}

auto Compiler::pop_scope() -> void {
  auto &locals = _targets.empty() ? _main_locals : _targets.top().locals;

  if (_targets.empty()) {
    _main_depth--;
  } else {
    _targets.top().depth--;
  }

  // while (locals.size() && locals.> depth) {
  //   decls.pop_back();
  //   emit(VM_POP);
  // }
}

auto Compiler::define_stack_var(std::string_view name, bool immut) -> bool {
  auto &locals = _targets.empty() ? _main_locals : _targets.top().locals;
  auto &depth  = _targets.empty() ? _main_depth : _targets.top().depth;

  if (depth == -1) return false;
  // locals.push_back({name, depth, immut});
  return true;
}

auto Compiler::get_stack_var(std::string_view name) -> const VarInfo * {
  auto &locals = _targets.empty() ? _main_locals : _targets.top().locals;
  // auto &decls  = locals.decls;

  // for (int i = decls.size() - 1; i >= 0; i--) {
  //   if (decls[i].name == name) {
  //     decls[i].slot = i;
  //     return &decls[i];
  //   }
  // }

  return nullptr;
}

auto Compiler::load_stack_var(std::uint16_t id) -> void {
  emit(VM_PSH);
  emit_arg(id, 2);
}

auto Compiler::store_stack_var(std::uint16_t id) -> void {
  emit(VM_STR);
  emit_arg(id, 2);
}

auto Compiler::get_upvalue(std::string_view name) -> const VarInfo * {
  if (_targets.size() < 2) return nullptr;

  // for (const auto &var : _targets.top().locals.decls) {
  //   if (var.name == name) return &var;
  // }

  return nullptr;
}

auto Compiler::load_upvalue(int depth, int slot) -> void {
}

auto Compiler::store_upvalue(int depth, int slot) -> void {
}

auto Compiler::encode_rodata(Value value) -> std::uint32_t {
  return write_rodata(&_program, value);
}

auto Compiler::load_rodata(std::uint32_t id) -> void {
  emit_op_arg(VM_VAL, id);
}

auto Compiler::encode_symbol(std::string_view str) -> std::uint32_t {
  // Check if the symbol is already in our symbol table
  if (_symbols.find(str) != _symbols.end()) { return _symbols[str]; }

  // If not, copy it into a VM allocated buffer
  char *c_str = (char *)memory(NULL, 0x0, str.size() + 1);
  memcpy(c_str, str.data(), str.size());
  c_str[str.size()] = '\0';

  // Make that buffer into a Symbol struct
  Symbol symbl;
  symbl.hash = hash(str.data());
  symbl.str  = c_str;

  // Add the symbol to our symbol table
  std::uint32_t symbol_id = write_symtable(&_program, symbl);
  _symbols[str]           = symbol_id;
  return symbol_id;
}

auto Compiler::define_symbol(std::uint32_t id) -> void {
  emit_op_arg(VM_DEF, id);
}

auto Compiler::load_symbol(std::uint32_t id) -> void {
  emit_op_arg(VM_SYM, id);
}

auto Compiler::assign_symbol(std::uint32_t id) -> void {
  emit_op_arg(VM_ASN, id);
}

auto Compiler::jmp_insert(std::uint8_t jmp_type) -> std::uint32_t {
  // Emit the jmp byte
  emit(jmp_type);

  // ... and a bit of padding for later
  emit(0x0);
  emit(0x0);

  // Return the current position in the bytecode
  // buffer for patching the jump later
  return get_offset();
}

auto Compiler::jmp_finish(std::uint32_t insc) -> void {
  // Calculate the difference between the jump and
  // current instruction
  std::uint16_t jmp_size = get_offset() - insc;

  // If we don't jump we can keep those 0's as padding
  if (jmp_size == 0) return;

  // Make sure our jump size is little endian
  if constexpr (IS_BIG_ENDIAN) jmp_size = SWAP_BYTES(jmp_size);

  // Patch our jump offset back into the padding we left earlier
  // in `Compiler::jmp_insert`
  get_buffer()[insc - 1] = (jmp_size >> 0) & 0xff;
  get_buffer()[insc - 2] = (jmp_size >> 8) & 0xff;
}

auto Compiler::logical_or(ASTNode &left, ASTNode &right) -> void {
  evaluate_expression(left);
  auto tjmp = jmp_insert(VM_JPT);

  evaluate_expression(right);
  auto orjmp = jmp_insert(VM_JMP);

  jmp_finish(tjmp);
  emit(VM_TRU);

  jmp_finish(orjmp);
}

auto Compiler::logical_and(ASTNode &left, ASTNode &right) -> void {
  // Evaluate the first operand and jump if it is false (1) ...
  evaluate_expression(left);
  auto fjmp = jmp_insert(VM_JPF);

  // If the first operand is true the value of the whole
  // expression is equal the truthiness of the second operand,
  // but we need to jump (2) ...
  evaluate_expression(right);
  auto andjmp = jmp_insert(VM_JMP);

  // (1) ... here, where we push false up on the stack
  jmp_finish(fjmp);
  emit(VM_FAL);

  // (2) ... here, so we don't push the previous false
  jmp_finish(andjmp);
}

auto Compiler::evaluate(ASTNode &parent, Identifier &node) -> void {
  auto varinfo = (const VarInfo *)nullptr;

  if (node.type() == Identifier::VAL) {

    if ((varinfo = get_stack_var(node.identifier()))) {
      load_stack_var(varinfo->slot);
    }

    if ((varinfo = get_upvalue(node.identifier()))) {
      load_upvalue(varinfo->depth, varinfo->slot);
    }

    if (_symbols.find(node.identifier()) != _symbols.end()) {
      return load_symbol(_symbols[node.identifier()]);
    }

    throw report(parent.location(), "undefined variable");
  } else {

    if ((varinfo = get_stack_var(node.identifier()))) {
      if (varinfo->immut) {
        throw report(parent.location(), "assignment to immutable variable");
      }

      return store_stack_var(varinfo->slot);
    }

    if ((varinfo = get_upvalue(node.identifier()))) {
      return store_upvalue(varinfo->depth, varinfo->slot);
    }

    throw report(parent.location(), "assignment to undefined variable");
  }
}

auto Compiler::evaluate(ASTNode &parent, Unary &node) -> void {
  // Push the operand to the stack
  evaluate_expression(node.operand());

  // ... and the corresponding operation
  switch (node.operation()) {
    case TokenKind::SYM_BANG: return emit(VM_NOT);
    case TokenKind::SYM_MINUS: return emit(VM_NEG);
    default: throw report(parent.location(), "invalid unary operation");
  }
}

auto Compiler::evaluate(ASTNode &parent, Binary &node) -> void {
  // Deal with short cirtcuiting operations seperately
  if (node.operation() == TokenKind::KW_OR) {
    logical_or(node.left(), node.right());
    return;
  } else if (node.operation() == TokenKind::KW_AND) {
    logical_and(node.left(), node.right());
    return;
  }

  // Push both expressions
  evaluate_expression(node.left());
  evaluate_expression(node.right());

  // And the corresponding operation
  switch (node.operation()) {
    case TokenKind::SYM_PLUS: return emit(VM_ADD);
    case TokenKind::SYM_MINUS: return emit(VM_SUB);
    case TokenKind::SYM_SLASH: return emit(VM_DIV);
    case TokenKind::SYM_STAR: return emit(VM_MUL);
    case TokenKind::SYM_SLASHSLASH: return emit(VM_RIV);
    case TokenKind::SYM_STARSTAR: return emit(VM_POW);
    case TokenKind::SYM_PERCENT: return emit(VM_MOD);

    case TokenKind::SYM_EQUALEQUAL: return emit(VM_EQ);
    case TokenKind::SYM_BANGEQUAL: return emit(VM_NEQ);
    case TokenKind::SYM_GT: return emit(VM_GT);
    case TokenKind::SYM_LT: return emit(VM_LT);
    case TokenKind::SYM_GTEQUAL: return emit(VM_GTE);
    case TokenKind::SYM_LTEQUAL: return emit(VM_LTE);

    default: throw report(parent.location(), "invalid binary operator");
  }
}

auto Compiler::evaluate(ASTNode &parent, KeyLiteral &node) -> void {
  switch (node.type()) {
    case KeyLiteral::PI: return emit(VM_PI);
    case KeyLiteral::TAU: return emit(VM_TAU);
    case KeyLiteral::EULER: return emit(VM_EUL);
    case KeyLiteral::VOID: return emit(VM_VID);
  }
}

auto Compiler::evaluate(ASTNode &parent, BoolLiteral &node) -> void {
  // There exist VM instructions for pushing boolean true and false
  emit(node.value() ? VM_TRU : VM_FAL);
}

auto Compiler::evaluate(ASTNode &parent, IntLiteral &node) -> void {
  size_t value_id;

  if (_integers.find(node.value()) != _integers.end()) {
    value_id = _integers[node.value()];
  } else {
    Value value;
    value.type       = T_INT;
    value.as.integer = node.value();

    value_id                = encode_rodata(value);
    _integers[node.value()] = value_id;
  }

  load_rodata(value_id);
}

auto Compiler::evaluate(ASTNode &parent, RealLiteral &node) -> void {
  size_t value_id;

  if (_reals.find(node.value()) != _reals.end()) {
    value_id = _reals[node.value()];
  } else {
    Value value;
    value.type    = T_REAL;
    value.as.real = node.value();

    value_id             = encode_rodata(value);
    _reals[node.value()] = value_id;
  }

  load_rodata(value_id);
}

auto Compiler::evaluate(ASTNode &parent, CharLiteral &node) -> void {

  // Check if the string is already in the read-only data
  if (_chars.find(node.value()) != _chars.end()) {
    load_rodata(_chars[node.value()]);
  }

  // Make it a VM Value
  Value value;
  value.type      = T_CHAR;
  value.as.charac = node.value();

  // Add the value to the read-only data
  size_t value_id      = encode_rodata(value);
  _chars[node.value()] = value_id;
  load_rodata(value_id);
}

auto Compiler::evaluate(ASTNode &parent, StringLiteral &node) -> void {
  // Check if the string is already in the read-only data
  if (_strings.find(node.value()) != _strings.end()) {
    load_rodata(_strings[node.value()]);
  }

  // Copy the string into a VM allocated buffer
  auto c_str =
    (char *)memory(NULL, 0x0, sizeof(char) * node.value().size() + 1);
  memcpy(c_str, node.value().data(), node.value().size());
  c_str[node.value().size()] = '\0';

  // Make it a VM Value
  Value value;
  value.type      = T_STR;
  value.as.string = c_str;

  // Add the value to the read-only data
  size_t value_id        = encode_rodata(value);
  _strings[node.value()] = value_id;
  load_rodata(value_id);
}

auto Compiler::evaluate(ASTNode &parent, ArrayLiteral &node) -> void {
  // ! This is not a good idea, this
  // ! can overflow the stack quite easily
  // ! but for now it works.

  // Push on the stack every element
  for (auto &elem : node.contents()) {
    evaluate_expression(elem);
  }

  // Pop off every element into an array
  emit_op_arg(VM_ARR, node.contents().size());
}

auto Compiler::evaluate(ASTNode &parent, VectorLiteral &node) -> void {
  // Make sure vector doesn't have more
  // than 256 components (max cardinality)
  if (node.contents().size() > std::numeric_limits<std::uint8_t>::max()) {
    throw report(parent.location(), "vector component limit reached");
  }

  // Push on the stack every component
  for (auto &comp : node.contents()) {
    evaluate_expression(comp);
  }

  // Pop off components in a vector
  emit_arg(VM_VEC, node.contents().size());
}

auto Compiler::evaluate(ASTNode &parent, Lambda &node) -> void {
  // Create a new target for the function and a scope
  // on that target
  _targets.emplace();
  push_scope();

  // Define the parameters of the functions on the stack
  for (const auto &param : node.parameters()) {
    define_stack_var(param.first, false);
  }

  // Compile the body of the function
  execute_statement(node.body());
  // ... and make sure the function returns no matter what
  emit(VM_RET);

  const auto &buffer = get_buffer();

  // add function value
  size_t fct_size     = sizeof(ObjectFunction) + sizeof(uint8_t) * get_offset();
  ObjectFunction *fct = (ObjectFunction *)memory(NULL, 0x0, fct_size);
  fct->obj.type       = O_FUNCTION;
  fct->len            = get_offset();

  std::memcpy(fct->bytes, buffer, fct->len);

  _targets.pop();

  Value value;
  value.type      = T_OBJ;
  value.as.object = (Object *)fct;

  auto val = encode_rodata(value);
  load_rodata(val);
}

auto Compiler::evaluate(ASTNode &parent, Assignment &node) -> void {
  evaluate_expression(node.target());
  evaluate_expression(node.value());
}

auto Compiler::evaluate(ASTNode &parent, Call &call) -> void {
  for (auto &arg : call.args()) {
    evaluate_expression(arg);
  }

  evaluate_expression(call.target());

  emit(VM_CAL);
  emit((std::uint8_t)call.args().size());
}

auto Compiler::evaluate(ASTNode &parent, Access &node) -> void {
  // TODO
}

auto Compiler::execute(Empty &) -> void {
  emit(VM_NOP);
}

auto Compiler::execute(Package &node) -> void {
  switch (node.action()) {
    case Package::DECLARE: {
      if (!_pkg_name.empty()) {
        throw report({0, 0}, "multiple package declarations");
      }

      _pkg_name = node.package();
      break;
    };

    case Package::IMPORT: {
      _pkg_imports.push_back(node.package());
      break;
    }
  }
}

auto Compiler::execute(Variable &node) -> void {
  evaluate_expression(node.init());
  if (!define_stack_var(node.name(), node.immut())) {
    throw report(node.init().location(), "cannot define variable");
  }
}

auto Compiler::execute(ExprStmt &node) -> void {
  evaluate_expression(node.expr());
  emit(VM_POP);
}

auto Compiler::execute(Block &node) -> void {
  push_scope();

  try {
    for (auto &child : node.statements()) {
      execute_statement(child);
    }
  } catch (...) {
    pop_scope();
    throw;
  }

  pop_scope();
}

auto Compiler::execute(Conditional &node) -> void {
  evaluate_expression(node.clause());

  // insert conditional jump
  auto tr = jmp_insert(VM_JPT);

  // compile instructions for false case
  execute_statement(node.altern());

  // unconditional jump over true case
  auto fl = jmp_insert(VM_JMP);

  // finish conditional jump
  jmp_finish(tr);

  // compile true case
  execute_statement(node.conseq());

  // finish false case
  jmp_finish(fl);

  // pop clause
  emit(VM_POP);
}

auto Compiler::execute(Loop &node) -> void {
  auto clause = get_offset();

  evaluate_expression(node.clause());

  auto out = jmp_insert(VM_JPF);

  emit(VM_POP);

  execute_statement(node.body());

  emit(VM_JBW);

  auto dist = get_offset() + 2 - clause;
  emit((dist >> 8) & 0xff);
  emit((dist >> 0) & 0xff);

  jmp_finish(out);

  emit(VM_POP);
}

auto Compiler::execute(Foreach &) -> void {
  // TODO
}

auto Compiler::execute(Match &) -> void {
  // TODO
}

auto Compiler::execute(MatchCase &) -> void {
  // TODO
}

auto Compiler::execute(ControlFlow &node) -> void {
  // TODO
}

auto Compiler::execute(Return &node) -> void {
  evaluate_expression(node.value());
  emit(VM_RETV);
}

auto Compiler::execute(Constant &node) -> void {
  auto id = encode_symbol(node.name());
  evaluate_expression(node.value());
  define_symbol(id);
}

auto Compiler::execute(Function &node) -> void {
  auto id = encode_symbol(node.name());
  evaluate_expression(node.lambda());
  define_symbol(id);
}

auto Compiler::execute(Enum &node) -> void {
  // TODO
}

auto Compiler::execute(Struct &node) -> void {
  // TODO
}

auto Compiler::execute(Main &node) -> void {
  // TODO
}

auto Compiler::compile(AST &ast) noexcept -> void {
  // Initialize the program with
  // 0 instructions, rodata or symbols
  init_program(&_program, 0, 0, 0);

  try {
    // Execute every declaration
    for (auto &node : ast) {
      execute_statement(node);
    }

    // Finalize the program
    emit(VM_FIN);
  } catch (...) {}
}
