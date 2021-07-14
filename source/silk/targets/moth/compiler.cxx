#include <silk/targets/moth/compiler.h>

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

#include <silk/language/syntax_tree.h>
#include <silk/language/token.h>

#define QUOTED(...) #__VA_ARGS__

namespace silk {

namespace moth {

const std::unordered_map<std::string_view, std::string_view> Compiler::modules =
  {
    {
      "io",
#include <stdsilk/io.silk>
    },
    {
      "bytes",
#include <stdsilk/bytes.silk>
    },
};

auto Compiler::emit(std::uint8_t byte) -> void {
  if (_targets.empty()) {
    write_byte(&_program, byte);
  } else {
    _targets.top().second.push_back(byte);
  }
}

auto Compiler::emit_varbyte_arg(std::uint32_t arg, std::size_t size) -> void {
  if constexpr (IS_BIG_ENDIAN) arg = SWAP_BYTES(arg);

  for (int i = size - 1; i >= 0; i--) {
    emit((arg >> (8 * i)) & 0xff);
  }
}

auto Compiler::emit_varbyte_op_arg(std::uint8_t base_op, std::uint32_t id)
  -> void {
  constexpr auto BYTE_MAX = std::numeric_limits<std::uint8_t>::max();

  if (id <= BYTE_MAX) {
    emit(base_op + 0);
    emit(id & 0xff);
  } else if (id <= BYTE_MAX * 2) {
    emit(base_op + 1);
    emit_varbyte_arg(id, 2);
  } else if (id <= BYTE_MAX * 3) {
    emit(base_op + 2);
    emit_varbyte_arg(id, 3);
  } else {
    emit(base_op + 3);
    emit_varbyte_arg(id, 4);
  }
}

auto Compiler::get_offset() -> std::uint32_t {
  if (_targets.empty()) {
    return _program.len;
  } else {
    return _targets.top().second.size();
  }
}

auto Compiler::get_buffer() -> std::uint8_t * {
  if (_targets.empty()) {
    return _program.bytes;
  } else {
    return _targets.top().second.data();
  }
}

auto Compiler::push_scope() -> void {
  if (_targets.empty()) {
    _main_locals.depth++;
  } else {
    _targets.top().first.depth++;
  }
}

auto Compiler::pop_scope() -> void {
  auto &locals = [&]() -> auto & {
    if (_targets.empty()) {
      _main_locals.depth--;
      return _main_locals;
    } else {
      _targets.top().first.depth--;
      return _targets.top().first;
    }
  }
  ();

  while (locals.definitions.size() &&
         locals.definitions.back().depth > locals.depth) {
    locals.definitions.pop_back();
    emit(VM_POP);
  }
}

auto Compiler::load_constant(std::uint64_t value) -> void {

  // Check if the char is already in the read-only data
  if (_naturals.find(value) != _naturals.end()) {
    load_rodata(_naturals[value]);
    return;
  }

  // Make it a VM Value
  Value v;
  v.type      = T_INT;
  v.as.charac = value;

  // Add the value to the read-only data
  auto value_id    = encode_rodata(v);
  _naturals[value] = value_id;

  load_rodata(value_id);
}

auto Compiler::load_constant(std::int64_t value) -> void {

  // Check if the integer is already in the read-only data
  if (_integers.find(value) != _integers.end()) {
    load_rodata(_integers[value]);
    return;
  }

  Value v;
  v.type       = T_INT;
  v.as.integer = value;

  auto value_id    = encode_rodata(v);
  _integers[value] = value_id;

  load_rodata(value_id);
}

auto Compiler::load_constant(double value) -> void {

  // Check if the real is already in the read-only data
  if (_reals.find(value) != _reals.end()) {
    load_rodata(_reals[value]);
    return;
  }

  Value v;
  v.type    = T_REAL;
  v.as.real = value;

  auto value_id = encode_rodata(v);
  _reals[value] = value_id;

  load_rodata(value_id);
}

auto Compiler::load_constant(wchar_t value) -> void {

  // Check if the char is already in the read-only data
  if (_chars.find(value) != _chars.end()) {
    load_rodata(_chars[value]);
    return;
  }

  // Make it a VM Value
  Value v;
  v.type      = T_CHAR;
  v.as.charac = value;

  // Add the value to the read-only data
  auto value_id = encode_rodata(v);
  _chars[value] = value_id;

  load_rodata(value_id);
}

auto Compiler::load_constant(std::string_view str) -> void {
  // Check if the string is already in the read-only data
  if (_strings.find(str) != _strings.end()) {
    load_rodata(_strings[str]);
    return;
  }

  // Copy the string into a VM allocated buffer
  auto c_str = (char *)memory(NULL, 0x0, sizeof(char) * str.size() + 1);
  memcpy(c_str, str.data(), str.size());
  c_str[str.size()] = '\0';

  // Make it a VM Value
  Value value;
  value.type      = T_STR;
  value.as.string = c_str;

  // Add the value to the read-only data
  auto value_id = encode_rodata(value);
  _strings[str] = value_id;
  load_rodata(value_id);
}

auto Compiler::load_identifier_val(const st::Node &node, std::string_view name)
  -> void {
  // ! URGENT: This function needs implementation

#ifdef pula
  if ((varinfo = get_stack_var(node.identifier()))) {
    load_stack_var(varinfo->slot);
  }

  if ((varinfo = get_upvalue(node.identifier()))) {
    load_upvalue(varinfo->depth, varinfo->slot);
  }

  if (_symbols.find(node.identifier()) != _symbols.end()) {
    return load_symbol(_symbols[node.identifier()]);
  }
#endif

  throw report("undefined variable", node.location);
}

auto Compiler::load_identifier_ref(const st::Node &node, std::string_view name)
  -> void {
  // ! URGENT: This function needs implementation

#ifdef pula
  if ((varinfo = get_stack_var(node.identifier()))) {
    if (varinfo->immut) {
      throw report(parent.location(), "assignment to immutable variable");
    }

    return store_stack_var(varinfo->slot);
  }

  if ((varinfo = get_upvalue(node.identifier()))) {
    return store_upvalue(varinfo->depth, varinfo->slot);
  }
#endif

  throw report("assignment to undefined variable", node.location);
}

auto Compiler::define_stack_var(std::string_view name, bool immut) -> bool {
  auto &locals = _targets.empty() ? _main_locals : _targets.top().first;

  if (locals.depth == -1) return false;

  locals.definitions.push_back({name, locals.depth, immut});
  return true;
}

auto Compiler::get_stack_var(std::string_view name)
  -> const std::pair<Definition, std::int64_t> {
  auto &locals = _targets.empty() ? _main_locals : _targets.top().first;

  for (auto i = 0; i < locals.definitions.size(); i++) {
    if (locals.definitions[i].name == name) {
      return {locals.definitions[i], i};
    }
  }

  return {{}, -1};
}

auto Compiler::load_stack_var(std::uint16_t slot) -> void {
  emit(VM_PSH);
  emit_varbyte_arg(slot, 2);
}

auto Compiler::store_stack_var(std::uint16_t slot) -> void {
  emit(VM_STR);
  emit_varbyte_arg(slot, 2);
}

auto Compiler::encode_rodata(Value value) -> std::uint32_t {
  return write_rodata(&_program, value);
}

auto Compiler::load_rodata(std::uint32_t id) -> void {
  emit_varbyte_op_arg(VM_VAL, id);
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
  emit_varbyte_op_arg(VM_DEF, id);
}

auto Compiler::load_symbol(std::uint32_t id) -> void {
  emit_varbyte_op_arg(VM_SYM, id);
}

auto Compiler::assign_symbol(std::uint32_t id) -> void {
  emit_varbyte_op_arg(VM_ASN, id);
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

auto Compiler::logical_or(st::Node &left, st::Node &right) -> void {
  handle_node(left);
  auto tjmp = jmp_insert(VM_JPT);

  handle_node(right);
  auto orjmp = jmp_insert(VM_JMP);

  jmp_finish(tjmp);
  emit(VM_TRU);

  jmp_finish(orjmp);
}

auto Compiler::logical_and(st::Node &left, st::Node &right) -> void {
  // Evaluate the first operand and jump if it is false (1) ...
  handle_node(left);
  auto fjmp = jmp_insert(VM_JPF);

  // If the first operand is true the value of the whole
  // expression is equal the truthiness of the second operand,
  // but we need to jump (2) ...
  handle_node(right);
  auto andjmp = jmp_insert(VM_JMP);

  // (1) ... here, where we push false up on the stack
  jmp_finish(fjmp);
  emit(VM_FAL);

  // (2) ... here, so we don't push the previous false
  jmp_finish(andjmp);
}

auto Compiler::handle(st::Node &, st::Comment &) -> void {
}

auto Compiler::handle(st::Node &, st::ModuleMain &) -> void {
}

auto Compiler::handle(st::Node &, st::ModuleDeclaration &) -> void {
}

auto Compiler::handle(st::Node &, st::ModuleImport &) -> void {
}

auto Compiler::handle(st::Node &, st::DeclarationFunction &) -> void {
}

auto Compiler::handle(st::Node &, st::DeclarationEnum &) -> void {
}

auto Compiler::handle(st::Node &, st::DeclarationObject &) -> void {
}

auto Compiler::handle(st::Node &, st::DeclarationExternLibrary &) -> void {
}

auto Compiler::handle(st::Node &, st::DeclarationExternFunction &) -> void {
}

auto Compiler::handle(st::Node &, st::DeclarationMacro &) -> void {
}

auto Compiler::handle(st::Node &, st::StatementEmpty &) -> void {
  emit(VM_NOP);
}

auto Compiler::handle(st::Node &, st::StatementExpression &data) -> void {
  handle_node(*data.child);
  emit(VM_POP);
}

auto Compiler::handle(st::Node &, st::StatementBlock &data) -> void {
  push_scope();

  try {
    for (auto &child : data.children) {
      handle_node(child);
    }
  } catch (...) {
    pop_scope();
    throw;
  }

  pop_scope();
}

auto Compiler::handle(st::Node &node, st::StatementCircuit &) -> void {
}

auto Compiler::handle(st::Node &node, st::StatementVariable &data) -> void {
  // evaluate_expression(node.init());
  // if (!define_stack_var(node.name(), node.immut())) {
  //   throw report(node.init().location(), "cannot define variable");
  // }
}

auto Compiler::handle(st::Node &node, st::StatementConstant &) -> void {
}

auto Compiler::handle(st::Node &node, st::StatementReturn &data) -> void {
  handle_node(*data.child);
  emit(VM_RET);
}

auto Compiler::handle(st::Node &node, st::StatementSwitch &data) -> void {
  emit(VM_RET);
}

auto Compiler::handle(st::Node &node, st::StatementIterationControl &data)
  -> void {
  // TODO
}

auto Compiler::handle(st::Node &node, st::StatementIf &data) -> void {
  handle_node(*data.condition);

  // insert conditional jump
  auto tr = jmp_insert(VM_JPT);

  if (data.alternative) {
    // compile instructions for false case
    handle_node(*data.alternative);
  }

  // unconditional jump over true case
  auto fl = jmp_insert(VM_JMP);

  // finish conditional jump
  jmp_finish(tr);

  // compile true case
  handle_node(*data.consequence);

  // finish false case
  jmp_finish(fl);

  // pop clause
  emit(VM_POP);
}

auto Compiler::handle(st::Node &node, st::StatementWhile &data) -> void {
  auto clause = get_offset();

  handle_node(*data.condition);

  auto out = jmp_insert(VM_JPF);

  emit(VM_POP);

  handle_node(*data.child);

  emit(VM_JBW);

  auto dist = get_offset() + 2 - clause;
  emit((dist >> 8) & 0xff);
  emit((dist >> 0) & 0xff);

  jmp_finish(out);

  emit(VM_POP);
}

auto Compiler::handle(st::Node &node, st::StatementLoop &) -> void {
  // TODO
}

auto Compiler::handle(st::Node &node, st::StatementFor &) -> void {
  // TODO
}

auto Compiler::handle(st::Node &node, st::StatementForeach &) -> void {
  // TODO
}

auto Compiler::handle(st::Node &node, st::StatementMatch &) -> void {
  // TODO
}

auto Compiler::handle(st::Node &node, st::ExpressionIdentifier &data) -> void {
  if (_assignment_context) {
    load_identifier_ref(node, data.name);
  } else {
    load_identifier_val(node, data.name);
  }
}

auto Compiler::handle(st::Node &node, st::ExpressionVoid &data) -> void {
  emit(OpCode::VM_VID);
}

auto Compiler::handle(st::Node &node, st::ExpressionContinuation &data)
  -> void {
  emit(OpCode::VM_VID);
}

auto Compiler::handle(st::Node &node, st::ExpressionBool &data) -> void {
  // There exist VM instructions for pushing boolean true and false
  emit(data.value ? VM_TRU : VM_FAL);
}

auto Compiler::handle(st::Node &node, st::ExpressionNat &data) -> void {
  // load_constant(data.value);
}

auto Compiler::handle(st::Node &node, st::ExpressionInt &data) -> void {
  load_constant(data.value);
}

auto Compiler::handle(st::Node &node, st::ExpressionReal &data) -> void {
  load_constant(data.value);
}

auto Compiler::handle(st::Node &node, st::ExpressionRealKeyword &data) -> void {
  switch (data.kind) {
    case st::ExpressionRealKeyword::PI: return emit(VM_PI);
    case st::ExpressionRealKeyword::TAU: return emit(VM_TAU);
    case st::ExpressionRealKeyword::EULER: return emit(VM_EUL);
  }
}

auto Compiler::handle(st::Node &node, st::ExpressionChar &data) -> void {
  load_constant(data.value);
}

auto Compiler::handle(st::Node &node, st::ExpressionString &data) -> void {
  load_constant(data.value);
}

auto Compiler::handle(st::Node &node, st::ExpressionTuple &data) -> void {
}

auto Compiler::handle(st::Node &node, st::ExpressionUnaryOp &data) -> void {
  // Push the operand to the stack
  handle_node(*data.child);

  // ... and the corresponding operation
  switch (data.kind) {
    case st::ExpressionUnaryOp::NOT: return emit(VM_NOT);
    case st::ExpressionUnaryOp::NEG: return emit(VM_NEG);
    default: throw report("invalid unary operation", node.location);
  }
}

auto Compiler::handle(st::Node &node, st::ExpressionBinaryOp &data) -> void {
  // Deal with short cirtcuiting operations seperately
  if (data.kind == data.OR) {
    logical_or(*data.left, *data.right);
    return;
  } else if (data.kind == data.AND) {
    logical_and(*data.left, *data.right);
    return;
  }

  // Push both expressions
  handle_node(*data.left);
  handle_node(*data.right);

  // And the corresponding operation
  switch (data.kind) {
    case st::ExpressionBinaryOp::ADD: return emit(VM_ADD);
    case st::ExpressionBinaryOp::SUB: return emit(VM_SUB);
    case st::ExpressionBinaryOp::DIV: return emit(VM_DIV);
    case st::ExpressionBinaryOp::MUL: return emit(VM_MUL);
    case st::ExpressionBinaryOp::RDIV: return emit(VM_RIV);
    case st::ExpressionBinaryOp::POW: return emit(VM_POW);
    case st::ExpressionBinaryOp::MOD: return emit(VM_MOD);

    case st::ExpressionBinaryOp::EQUAL: return emit(VM_EQ);
    case st::ExpressionBinaryOp::NOTEQ: return emit(VM_NEQ);
    case st::ExpressionBinaryOp::LESS: return emit(VM_LT);
    case st::ExpressionBinaryOp::LESSEQ: return emit(VM_LTE);
    case st::ExpressionBinaryOp::GREATER: return emit(VM_GT);
    case st::ExpressionBinaryOp::GREATEREQ: return emit(VM_GTE);

    case st::ExpressionBinaryOp::APPEND: return emit(VM_MRG);
    case st::ExpressionBinaryOp::MERGE: return emit(VM_MRG);
    case st::ExpressionBinaryOp::INDEX: {
      if (!_assignment_context) emit(VM_IDX);
      return;
    }

    default: throw report("invalid binary operator", node.location);
  }
}

auto Compiler::handle(st::Node &node, st::ExpressionRange &data) -> void {
}

auto Compiler::handle(st::Node &node, st::ExpressionVector &data) -> void {
  // Make sure vector doesn't have more
  // than 256 components (max cardinality)
  // if (node.contents().size() > std::numeric_limits<std::uint8_t>::max()) {
  //   throw report(parent.location(), "vector component limit reached");
  // }

  // // Push on the stack every component
  // for (auto &comp : node.contents()) {
  //   evaluate_expression(comp);
  // }

  // // Pop off components in a vector
  // emit_arg(VM_VEC, node.contents().size());
}

auto Compiler::handle(st::Node &node, st::ExpressionArray &data) -> void {
  // Create a new array on the stack
  emit(VM_ARR);

  // Push each element of the array, along with it's index
  // and then perform an index assignment
  // for (auto i = std::int64_t{0}; i < node.contents().size(); i++) {
  //   load_constant(i);
  //   evaluate_expression(node.contents()[i]);
  //   emit(VM_IDA);
  // }
}

auto Compiler::handle(st::Node &node, st::ExpressionDictionary &data) -> void {
  // Create a new dictionary on the stack
  emit(VM_DCT);

  // Push each value of the dictonary, along with it's key
  // and then perform an index assignment
  // for (auto &[key, val] : node.contents()) {
  //   evaluate_expression(val);
  //   evaluate_expression(key);
  //   emit(VM_IDA);
  // }
}

auto Compiler::handle(st::Node &node, st::ExpressionAssignment &data) -> void {
  _assignment_context = true;
  handle_node(*data.assignee);
  _assignment_context = false;
  handle_node(*data.child);
}

auto Compiler::handle(st::Node &node, st::ExpressionCall &data) -> void {
  // for (auto &arg : call.args()) {
  //   evaluate_expression(arg);
  // }

  // evaluate_expression(call.target());

  // emit(VM_CAL);
  // emit((std::uint8_t)call.args().size());
}

auto Compiler::handle(st::Node &node, st::ExpressionLambda &data) -> void {
  // Create a new target for the function and a scope
  // on that target
  // _targets.emplace();
  // push_scope();

  // // Define the parameters of the functions on the stack
  // for (const auto &param : node.parameters()) {
  //   define_stack_var(param.first, false);
  // }

  // // Compile the body of the function
  // execute_statement(node.body());
  // // ... and make sure the function returns no matter what
  // emit(VM_RET);

  // const auto &buffer = get_buffer();

  // // add function value
  // size_t fct_size     = sizeof(ObjectFunction) + sizeof(uint8_t) *
  // get_offset(); ObjectFunction *fct = (ObjectFunction *)memory(NULL, 0x0,
  // fct_size); fct->obj.type       = O_FUNCTION; fct->len            =
  // get_offset();

  // std::memcpy(fct->bytes, buffer, fct->len);

  // _targets.pop();

  // Value value;
  // value.type      = T_OBJ;
  // value.as.object = (::Object *)fct;

  // auto val = encode_rodata(value);
  // load_rodata(val);
}

auto Compiler::add_breakpoint(size_t) noexcept -> void {
}

auto Compiler::add_breakpoint(std::string) noexcept -> void {
}

auto Compiler::execute(Package &&pkg) noexcept -> Program {
  // Initialize the program with
  // 0 instructions, rodata or symbols
  init_program(&_program, 0, 0, 0);

  // try {
  //   // Execute every declaration
  //   for (auto &node : ast) {
  //     execute_statement(node);
  //   }
  // } catch (...) {}

  return _program;
}

} // namespace moth

} // namespace silk
