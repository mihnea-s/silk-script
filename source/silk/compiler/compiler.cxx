#include <silk/compiler/compiler.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>

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

#include <silk/lexer/token.h>
#include <silk/parser/ast.h>
#include <silk/util/error.h>

auto Compiler::emit(std::uint8_t byte) -> void {
  if (_targets.empty()) {
    write_byte(&_program, byte);
  } else {
    _targets.top().buffer.push_back(byte);
  }
}

auto Compiler::argx(std::uint32_t arg, std::size_t size) -> void {
  if constexpr (IS_BIG_ENDIAN) arg = SWAP_BYTES(arg);

  for (int i = size - 1; i >= 0; i--) {
    emit((arg >> (8 * i)) & 0xff);
  }
}

auto Compiler::argx_op(std::uint8_t base_op, std::uint32_t id) -> void {
  if (id <= UINT8_MAX) {
    emit(base_op + 0);
    emit(id & 0xff);
  } else if (id <= UINT16_MAX) {
    emit(base_op + 1);
    argx(id, 2);
  } else if (id <= UINT8_MAX * 3) {
    emit(base_op + 2);
    argx(id, 3);
  } else {
    emit(base_op + 3);
    argx(id, 4);
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
    _main_locals.depth++;
  } else {
    _targets.top().locals.depth++;
  }
}

auto Compiler::pop_scope() -> void {
  auto &locals = _targets.empty() ? _main_locals : _targets.top().locals;
  auto &depth  = locals.depth;
  auto &decls  = locals.decls;

  depth--;

  while (decls.size() && decls.back().depth > depth) {
    decls.pop_back();
    emit(VM_POP);
  }
}

auto Compiler::define_stack_var(std::string_view name, bool is_const) -> bool {
  auto &locals = _targets.empty() ? _main_locals : _targets.top().locals;
  auto &depth  = locals.depth;
  auto &decls  = locals.decls;

  if (depth == -1) return false;
  decls.push_back({name, depth, is_const});
  return true;
}

auto Compiler::get_stack_var(std::string_view name) -> const Varinfo * {
  auto &locals = _targets.empty() ? _main_locals : _targets.top().locals;
  auto &decls  = locals.decls;

  for (int i = decls.size() - 1; i >= 0; i--) {
    if (decls[i].name == name) {
      decls[i].slot = i;
      return &decls[i];
    }
  }

  return nullptr;
}

auto Compiler::load_stack_var(std::uint16_t id) -> void {
  emit(VM_PSH);
  argx(id, 2);
}

auto Compiler::store_stack_var(std::uint16_t id) -> void {
  emit(VM_STR);
  argx(id, 2);
}

auto Compiler::get_upvalue(std::string_view name) -> const Varinfo * {
  if (_targets.size() < 2) return nullptr;

  // for (const auto& var : _targets.top().locals.decls) {
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
  argx_op(VM_VAL, id);
}

auto Compiler::encode_symbol(Symbol symbol) -> std::uint32_t {
  return write_symtable(&_program, symbol);
}

auto Compiler::encode_symbol(std::string_view str) -> std::uint32_t {
  std::uint32_t symbol_id = 0;

  if (_symbols.find(str) != _symbols.end()) {
    symbol_id = _symbols[str];
  } else {
    char *c_str = (char *)memory(NULL, 0x0, str.size() + 1);
    memcpy(c_str, str.data(), str.size());
    c_str[str.size()] = '\0';

    Symbol symbl;
    symbl.hash = hash(str.data());
    symbl.str  = c_str;

    symbol_id     = encode_symbol(symbl);
    _symbols[str] = symbol_id;
  }

  return symbol_id;
}

auto Compiler::define_symbol(uint32_t id) -> void {
  argx_op(VM_DEF, id);
}

auto Compiler::load_symbol(std::uint32_t id) -> void {
  argx_op(VM_SYM, id);
}

auto Compiler::assign_symbol(uint32_t id) -> void {
  argx_op(VM_ASN, id);
}

auto Compiler::jmp_insert(std::uint8_t type) -> std::uint32_t {
  emit(type);
  emit(0x0);
  emit(0x0);
  return get_offset();
}

auto Compiler::jmp_finish(std::uint32_t insc) -> void {
  std::uint16_t jmp_size = get_offset() - insc;

  if (jmp_size == 0) return;

  if constexpr (IS_BIG_ENDIAN) jmp_size = SWAP_BYTES(jmp_size);

  get_buffer()[insc - 1] = (jmp_size >> 0) & 0xff;
  get_buffer()[insc - 2] = (jmp_size >> 8) & 0xff;
}

auto Compiler::evaluate(ASTNode &parent, Identifier &node) -> void {
  // TODO
  //   const Varinfo* varinfo = {nullptr};

  //   if (!(varinfo = get_stack_var(node.identifier))) {

  //   if (!(varinfo = get_upvalue(node.identifier))) {

  //   }
  //   }

  //     load_stack_var(varinfo->slot);
  //     load_upvalue(varinfo->depth, varinfo->slot);

  //   if (_symbols.find(node.identifier) != _symbols.end()) {
  //     return load_symbol(_symbols[node.identifier]);
  //   }

  //   const Varinfo* varinfo = nullptr;

  //   if ((varinfo = get_stack_var(node.identifier))) {
  //     if (varinfo->is_const && node.which == Identifier::REF) {
  //       throw report({0, 0}, "assignment to const variable");
  //     }

  //     return store_stack_var(varinfo->slot);
  //   }

  //   if ((varinfo = get_upvalue(node.identifier))) {
  //     return store_upvalue(varinfo->depth, varinfo->slot);
  //   }

  //   throw report_error(node.location,
  //   SilkErrors::undefAssign(node.identifier));

  //   throw report_error(node.location,
  //   SilkErrors::undefUsage(node.identifier));
}

auto Compiler::evaluate(ASTNode &parent, Unary &node) -> void {
  evaluate_expression(node.operand());

  switch (node.operation()) {
    case TokenType::SYM_BANG: return emit(VM_NOT);
    case TokenType::SYM_MINUS: return emit(VM_NEG);
    default: throw report(parent.location(), "invalid unary operation");
  }
}

auto Compiler::evaluate(ASTNode &parent, Binary &node) -> void {
  evaluate_expression(node.left());
  evaluate_expression(node.right());

  switch (node.operation()) {
    case TokenType::SYM_PLUS: return emit(VM_ADD);
    case TokenType::SYM_MINUS: return emit(VM_SUB);
    case TokenType::SYM_SLASH: return emit(VM_DIV);
    case TokenType::SYM_STAR: return emit(VM_MUL);
    case TokenType::SYM_SLASHSLASH: return emit(VM_RIV);
    case TokenType::SYM_STARSTAR: return emit(VM_POW);
    case TokenType::SYM_PERCENT: return emit(VM_MOD);

    case TokenType::SYM_EQUALEQUAL: return emit(VM_EQ);
    case TokenType::SYM_BANGEQUAL: return emit(VM_NEQ);
    case TokenType::SYM_GT: return emit(VM_GT);
    case TokenType::SYM_LT: return emit(VM_LT);
    case TokenType::SYM_GTEQUAL: return emit(VM_GTE);
    case TokenType::SYM_LTEQUAL: return emit(VM_LTE);

    default: throw report(parent.location(), "invalid binary operator");
  }
}

auto Compiler::evaluate(ASTNode &parent, BoolLiteral &node) -> void {
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
  // TODO
}

auto Compiler::evaluate(ASTNode &parent, StringLiteral &node) -> void {
  size_t value_id;

  if (_strings.find(node.value()) != _strings.end()) {
    value_id = _strings[node.value()];
  } else {
    auto c_str =
      (char *)memory(NULL, 0x0, sizeof(char) * node.value().size() + 1);
    memcpy(c_str, node.value().data(), node.value().size());
    c_str[node.value().size()] = '\0';

    Value value;
    value.type      = T_STR;
    value.as.string = c_str;

    value_id               = encode_rodata(value);
    _strings[node.value()] = value_id;
  }

  load_rodata(value_id);
}

auto Compiler::evaluate(ASTNode &parent, ArrayLiteral &node) -> void {
  // TODO
}

auto Compiler::evaluate(ASTNode &parent, Constant &node) -> void {
  switch (node.which()) {
    case Constant::PI: return emit(VM_PI);
    case Constant::TAU: return emit(VM_TAU);
    case Constant::EULER: return emit(VM_EUL);
    case Constant::VOID: return emit(VM_VID);
  }
}

auto Compiler::evaluate(ASTNode &parent, Lambda &node) -> void {
  // TODO
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

auto Compiler::evaluate(ASTNode &parent, ConstExpr &node) -> void {
  // TODO
}

auto Compiler::execute(Empty &) -> void {
  emit(VM_NOP);
}

auto Compiler::execute(Package &node) -> void {
  switch (node.action()) {
    case Package::DECLARE: {
      if (!_pkginfo.name.empty()) {
        throw report({0, 0}, "multiple package declarations");
      }

      _pkginfo.name = node.package();
      if (node.package() == "main") push_scope();
      break;
    };

    case Package::IMPORT: {
      _pkginfo.imports.push_back(node.package());
      break;
    }
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
  emit(VM_RET);
}

auto Compiler::execute(Variable &node) -> void {
  evaluate_expression(node.init());
  if (!define_stack_var(node.name(), node.immut())) {
    throw report(node.init().location(), "cannot define variable");
  }
}

auto Compiler::execute(Function &node) -> void {
  auto id = encode_symbol(node.name());

  _targets.emplace();

  push_scope();
  for (const auto &param : node.lambda().parameters()) {
    define_stack_var(param.first, false);
  }

  execute_statement(node.lambda().body());

  emit(VM_VID);
  emit(VM_RET);

  const auto &buf = _targets.top().buffer;

  // add function value
  size_t fct_size     = sizeof(ObjectFunction) + sizeof(uint8_t) * buf.size();
  ObjectFunction *fct = (ObjectFunction *)memory(NULL, 0x0, fct_size);
  fct->obj.type       = O_FUNCTION;
  fct->len            = buf.size();

  std::memcpy(fct->bytes, buf.data(), fct->len);

  _targets.pop();

  Value value;
  value.type      = T_OBJ;
  value.as.object = (Object *)fct;

  auto val = encode_rodata(value);
  load_rodata(val);

  define_symbol(id);
}

auto Compiler::execute(Enum &node) -> void {
  // TODO
}

auto Compiler::execute(Struct &node) -> void {
  // TODO
}

// public compile function

auto Compiler::compile(AST &ast) noexcept -> void {
  init_program(&_program, 0, 0, 0);

  try {
    for (auto &node : ast) {
      execute_statement(node);
    }

    if (_pkginfo.name == "main") pop_scope();

    emit(VM_FIN);
  } catch (...) {}
}

auto Compiler::write_to_file(std::string_view file) noexcept -> void {
  const char *err = nullptr;
  write_file(file.data(), &_program, &err);
  if (err) report({0, 0}, err);
}

auto Compiler::run_in_vm(VM *vm) noexcept -> VMStatus {
  run(vm, &_program);
  return vm->st;
}

auto Compiler::free_program() noexcept -> void {
  ::free_program(&_program);
}