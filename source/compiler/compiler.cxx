#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <vm/file.h>
#include <vm/macros.h>
#include <vm/mem.h>
#include <vm/object.h>
#include <vm/opcode.h>
#include <vm/program.h>
#include <vm/rodata.h>
#include <vm/symtable.h>
#include <vm/value.h>

#include <silk/common/ast.h>
#include <silk/common/error.h>
#include <silk/common/errors.h>
#include <silk/common/token.h>
#include <silk/compiler/compiler.h>

auto Compiler::emit(std::uint8_t byte) -> void {
  if (_target.is_function) {
    ;
    return;
  }

  write_ins(&_program, byte);
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

auto Compiler::load_val(std::uint32_t id) -> void {
  argx_op(VM_VAL, id);
}

auto Compiler::load_var(std::uint8_t id) -> void {
  emit(VM_PSH);
  emit(id);
}

auto Compiler::define_sym(uint32_t id) -> void {
  argx_op(VM_DEF, id);
}

auto Compiler::load_sym(std::uint32_t id) -> void {
  argx_op(VM_SYM, id);
}

auto Compiler::assign_sym(uint32_t id) -> void {
  argx_op(VM_ASN, id);
}

auto Compiler::jmp_insert(std::uint8_t type) -> std::uint32_t {
  emit(type);
  emit(0x0);
  emit(0x0);
  return _program.len;
}

auto Compiler::jmp_finish(std::uint32_t insc) -> void {
  std::uint16_t jmp_size = _program.len - insc;

  if (jmp_size == 0) return;

  if constexpr (IS_BIG_ENDIAN) jmp_size = SWAP_BYTES(jmp_size);

  _program.ins[insc - 1] = (jmp_size >> 0) & 0xff;
  _program.ins[insc - 2] = (jmp_size >> 8) & 0xff;
}

auto Compiler::push_scope() -> void {
  _depth++;
}

auto Compiler::pop_scope() -> void {
  _depth--;
  while (_variables.size() && _variables.back().depth > _depth) {
    _variables.pop_back();
    emit(VM_POP);
  }
}

auto Compiler::to_scope(std::string_view name, bool is_const) -> bool {
  if (_depth == -1) return false;
  _variables.push_back({name, _depth, is_const});
  return true;
}

auto Compiler::from_scope(std::string_view name) const
  -> const std::pair<_varinfo, int> {
  for (int i = _variables.size() - 1; i >= 0; i--) {
    if (_variables[i].name == name) { return {_variables[i], i}; }
  }

  return {{}, -1};
}

auto Compiler::encode_rodata(Value value) -> std::uint32_t {
  return write_rod(&_program, value);
}

auto Compiler::encode_symbol(Symbol symbol) -> std::uint32_t {
  return write_sym(&_program, symbol);
}

auto Compiler::encode_symbol_from_str(std::string_view str) -> std::uint32_t {
  std::uint32_t symbol_id = 0;

  if (_symbols.find(str) != _symbols.end()) {
    symbol_id = _symbols[str];
  } else {
    char* c_str = (char*)memory(NULL, 0x0, str.size() + 1);
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

auto Compiler::evaluate(const Unary& node) -> void {
  visit_node(node.operand);
  switch (node.operation) {
    case TokenType::sym_bang: return emit(VM_NOT);
    case TokenType::sym_minus: return emit(VM_NEG);
    case TokenType::kw_typeof: return emit(VM_TYP);

    default:
      throw report_error(node.location, SilkErrors::invalidOperand("unary"));
  }
}

auto Compiler::evaluate(const Binary& node) -> void {
  visit_node(node.left);
  visit_node(node.right);

  switch (node.operation) {
    case TokenType::sym_plus: return emit(VM_ADD);
    case TokenType::sym_minus: return emit(VM_SUB);
    case TokenType::sym_slash: return emit(VM_DIV);
    case TokenType::sym_star: return emit(VM_MUL);
    case TokenType::sym_slashslash: return emit(VM_RIV);
    case TokenType::sym_starstar: return emit(VM_POW);
    case TokenType::sym_percent: return emit(VM_MOD);

    case TokenType::kw_is: return emit(VM_IS);
    case TokenType::kw_isnt: return emit(VM_ISN);

    case TokenType::sym_equalequal: return emit(VM_EQ);
    case TokenType::sym_bangequal: return emit(VM_NEQ);
    case TokenType::sym_gt: return emit(VM_GT);
    case TokenType::sym_lt: return emit(VM_LT);
    case TokenType::sym_gtequal: return emit(VM_GTE);
    case TokenType::sym_ltequal: return emit(VM_LTE);

    default:
      throw report_error(node.location, SilkErrors::invalidOperand("binary"));
  }
}

auto Compiler::evaluate(const IntLiteral& node) -> void {
  size_t value_id;

  if (_integers.find(node.value) != _integers.end()) {
    value_id = _integers[node.value];
  } else {
    Value value;
    value.type       = T_INT;
    value.as.integer = node.value;

    value_id              = encode_rodata(value);
    _integers[node.value] = value_id;
  }

  load_val(value_id);
}

auto Compiler::evaluate(const RealLiteral& node) -> void {
  size_t value_id;

  if (_reals.find(node.value) != _reals.end()) {
    value_id = _reals[node.value];
  } else {
    Value value;
    value.type    = T_REAL;
    value.as.real = node.value;

    value_id           = encode_rodata(value);
    _reals[node.value] = value_id;
  }

  load_val(value_id);
}

auto Compiler::evaluate(const StringLiteral& node) -> void {
  size_t value_id;

  if (_strings.find(node.value) != _strings.end()) {
    value_id = _strings[node.value];
  } else {
    auto c_str = (char*)memory(NULL, 0x0, sizeof(char) * node.value.size() + 1);
    memcpy(c_str, node.value.data(), node.value.size());
    c_str[node.value.size()] = '\0';

    Value value;
    value.type      = T_STR;
    value.as.string = c_str;

    value_id             = encode_rodata(value);
    _strings[node.value] = value_id;
  }

  load_val(value_id);
}

auto Compiler::evaluate(const BoolLiteral& node) -> void {
  emit(node.value ? VM_TRU : VM_FAL);
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

auto Compiler::evaluate(const Lambda& node) -> void {
}

auto Compiler::evaluate(const ConstExpr& node) -> void {
}

auto Compiler::evaluate(const Assignment& node) -> void {
  visit_node(node.assignment);
  visit_node(node.target);
}

auto Compiler::evaluate(const IdentifierVal& node) -> void {
  auto varinfo = from_scope(node.identifier);
  if (varinfo.second == -1) {
    if (_symbols.find(node.identifier) == _symbols.end()) {
      throw report_error(
        node.location, SilkErrors::undefUsage(node.identifier));
    }

    load_sym(_symbols[node.identifier]);
    return;
  }

  load_var(varinfo.second);
}

auto Compiler::evaluate(const IdentifierRef& node) -> void {
  auto varinfo = from_scope(node.identifier);

  if (varinfo.second == -1) {
    throw report_error(node.location, SilkErrors::undefAssign(node.identifier));
  }

  if (varinfo.first.is_const) {
    throw report_error(node.location, SilkErrors::constAssign(node.identifier));
  }

  emit(VM_STR);
  emit(varinfo.second);
}

auto Compiler::evaluate(const Grouping& node) -> void {
}

auto Compiler::evaluate(const Call& node) -> void {
  visit_node(node.target);
  emit(VM_CAL);
}

auto Compiler::evaluate(const Access& node) -> void {
}

auto Compiler::execute(const Empty&) -> void {
  emit(VM_NOP);
}

auto Compiler::execute(const Package& node) -> void {
  switch (node.action) {
    case Package::DECLARATION: {
      if (!_pkginfo.name.empty()) {
        throw report_error(node.location, SilkErrors::multiPkg());
      }

      _pkginfo.name = node.package;
      if (node.package == "main") push_scope();
      break;
    };

    case Package::IMPORT: {
      _pkginfo.imports.push_back(node.package);
      break;
    }
  }
}

auto Compiler::execute(const Variable& node) -> void {
  visit_node(node.initializer);
  if (!to_scope(node.name, node.is_constant)) {
    throw report_error(node.location, SilkErrors::noScope());
  }
}

auto Compiler::execute(const Function& node) -> void {
  _target.is_function = true;
  _target.buffer.clear();

  visit_node(node.body);
  emit(VM_VID);

  _target.is_function = false;

  // add function value
  size_t fct_size =
    sizeof(ObjectFunction) + sizeof(uint8_t) * _target.buffer.size();
  ObjectFunction* fct = (ObjectFunction*)memory(NULL, 0x0, fct_size);
  fct->obj.type       = O_FUNCTION;
  fct->len            = _target.buffer.size();

  std::memcpy(fct->ins, _target.buffer.data(), fct->len);

  Value value;
  value.type      = T_OBJ;
  value.as.object = (Object*)fct;

  auto val = encode_rodata(value);
  load_val(val);

  auto id = encode_symbol_from_str(node.name);
  define_sym(id);
}

auto Compiler::execute(const Struct& node) -> void {
}

auto Compiler::execute(const Loop& node) -> void {
  auto clause = _program.len;

  visit_node(node.clause);

  auto out = jmp_insert(VM_JPF);

  emit(VM_POP);

  visit_node(node.body);

  emit(VM_JBW);

  auto dist = _program.len + 2 - clause;
  emit((dist >> 8) & 0xff);
  emit((dist >> 0) & 0xff);

  jmp_finish(out);

  emit(VM_POP);
}

auto Compiler::execute(const Conditional& node) -> void {
  visit_node(node.clause);

  // insert conditional jump
  auto tr = jmp_insert(VM_JPT);

  // compile instructions for false case
  if (node.if_false) visit_node(node.if_false);

  // unconditional jump over true case
  auto fl = jmp_insert(VM_JMP);

  // finish conditional jump
  jmp_finish(tr);

  // compile true case
  visit_node(node.if_true);

  // finish false case
  jmp_finish(fl);

  // pop clause
  emit(VM_POP);
}

auto Compiler::execute(const Match&) -> void {
}

auto Compiler::execute(const MatchCase&) -> void {
}

auto Compiler::execute(const Block& node) -> void {
  push_scope();

  try {
    for (const auto& child : node.body) {
      visit_node(child);
    }
  } catch (...) {
    pop_scope();
    throw;
  }

  pop_scope();
}

auto Compiler::execute(const Interrupt& node) -> void {
}

auto Compiler::execute(const Return& node) -> void {
  if (node.value) visit_node(node.value);
  emit(VM_RET);
}

auto Compiler::execute(const ExprStmt& node) -> void {
  visit_node(node.expression);
  emit(VM_POP);
}

// public compile function

auto Compiler::compile(const AST& ast) noexcept -> void {
  init_program(&_program, 0, 0, 0);

  try {
    for (const auto& node : ast.program) {
      visit_node(node);
    }

    if (_pkginfo.name == "main") pop_scope();

    emit(VM_FIN);
  } catch (...) {}
}

auto Compiler::write_to_file(std::string_view file) noexcept -> void {
  const char* err = nullptr;
  write_file(file.data(), &_program, &err);
  if (err) report_error({0, 0}, err);
}
