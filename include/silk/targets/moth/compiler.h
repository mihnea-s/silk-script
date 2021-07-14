#pragma once

#include <stack>

#include <moth/program.h>
#include <moth/value.h>
#include <moth/vm.h>

#include <silk/language/package.h>
#include <silk/language/syntax_tree.h>
#include <silk/pipeline/stage.h>

namespace silk {

namespace moth {

class Compiler final : public Stage<Compiler, Package, Program> {
private:
  // Standard library modules
  static const std::unordered_map<std::string_view, std::string_view> modules;

  //
  struct Definition {
    std::string_view name;
    int              depth;
    bool             immutable;
  };

  //
  using Definitions = std::vector<Definition>;

  //
  struct Locals {
    int         depth = -1;
    Definitions definitions;
  };

  //
  using Target = std::pair<Locals, std::vector<uint8_t>>;

  // Package information
  std::string_view              _pkg_name    = {};
  std::vector<std::string_view> _pkg_imports = {};

  // Bytecode
  Program _program;

  // Global constants
  std::vector<std::string_view> _globals = {};

  // Main locals
  Locals _main_locals = {};

  // If compiling in an assigning context
  bool _assignment_context = false;

  // Function targets
  std::stack<Target> _targets = {};

  // Caching the id of read
  std::unordered_map<std::uint64_t, std::uint32_t>    _naturals = {};
  std::unordered_map<std::int64_t, std::uint32_t>     _integers = {};
  std::unordered_map<double, std::uint32_t>           _reals    = {};
  std::unordered_map<wchar_t, std::uint32_t>          _chars    = {};
  std::unordered_map<std::string_view, std::uint32_t> _strings  = {};
  std::unordered_map<std::string_view, std::uint32_t> _symbols  = {};

  // bytecode functions
  auto emit(std::uint8_t) -> void;
  auto emit_varbyte_arg(std::uint32_t, std::size_t) -> void;
  auto emit_varbyte_op_arg(std::uint8_t, std::uint32_t) -> void;

  auto get_offset() -> std::uint32_t;
  auto get_buffer() -> std::uint8_t *;

  auto push_scope() -> void;
  auto pop_scope() -> void;

  auto load_constant(std::uint64_t) -> void;
  auto load_constant(std::int64_t) -> void;
  auto load_constant(double) -> void;
  auto load_constant(wchar_t) -> void;
  auto load_constant(std::string_view) -> void;

  auto load_identifier_val(const st::Node &, std::string_view) -> void;
  auto load_identifier_ref(const st::Node &, std::string_view) -> void;

  auto define_stack_var(std::string_view, bool) -> bool;
  auto get_stack_var(std::string_view) -> const std::pair<Definition, int64_t>;
  auto load_stack_var(std::uint16_t) -> void;
  auto store_stack_var(std::uint16_t) -> void;

  auto encode_rodata(Value) -> std::uint32_t;
  auto load_rodata(std::uint32_t) -> void;

  auto encode_symbol(std::string_view) -> std::uint32_t;
  auto load_symbol(std::uint32_t) -> void;
  auto define_symbol(std::uint32_t) -> void;
  auto assign_symbol(std::uint32_t) -> void;

  auto jmp_insert(std::uint8_t) -> std::uint32_t;
  auto jmp_finish(std::uint32_t) -> void;

  auto logical_or(st::Node &, st::Node &) -> void;
  auto logical_and(st::Node &, st::Node &) -> void;

  auto handle(st::Node &, st::Comment &) -> void override;
  auto handle(st::Node &, st::ModuleMain &) -> void override;
  auto handle(st::Node &, st::ModuleDeclaration &) -> void override;
  auto handle(st::Node &, st::ModuleImport &) -> void override;

  auto handle(st::Node &, st::DeclarationFunction &) -> void override;
  auto handle(st::Node &, st::DeclarationEnum &) -> void override;
  auto handle(st::Node &, st::DeclarationObject &) -> void override;
  auto handle(st::Node &, st::DeclarationExternLibrary &) -> void override;
  auto handle(st::Node &, st::DeclarationExternFunction &) -> void override;
  auto handle(st::Node &, st::DeclarationMacro &) -> void override;

  auto handle(st::Node &, st::StatementEmpty &) -> void override;
  auto handle(st::Node &, st::StatementExpression &) -> void override;
  auto handle(st::Node &, st::StatementBlock &) -> void override;
  auto handle(st::Node &, st::StatementCircuit &) -> void override;
  auto handle(st::Node &, st::StatementVariable &) -> void override;
  auto handle(st::Node &, st::StatementConstant &) -> void override;
  auto handle(st::Node &, st::StatementReturn &) -> void override;
  auto handle(st::Node &, st::StatementSwitch &) -> void override;
  auto handle(st::Node &, st::StatementIterationControl &) -> void override;
  auto handle(st::Node &, st::StatementIf &) -> void override;
  auto handle(st::Node &, st::StatementWhile &) -> void override;
  auto handle(st::Node &, st::StatementLoop &) -> void override;
  auto handle(st::Node &, st::StatementFor &) -> void override;
  auto handle(st::Node &, st::StatementForeach &) -> void override;
  auto handle(st::Node &, st::StatementMatch &) -> void override;

  auto handle(st::Node &, st::ExpressionIdentifier &) -> void override;
  auto handle(st::Node &, st::ExpressionVoid &) -> void override;
  auto handle(st::Node &, st::ExpressionContinuation &) -> void override;
  auto handle(st::Node &, st::ExpressionBool &) -> void override;
  auto handle(st::Node &, st::ExpressionNat &) -> void override;
  auto handle(st::Node &, st::ExpressionInt &) -> void override;
  auto handle(st::Node &, st::ExpressionReal &) -> void override;
  auto handle(st::Node &, st::ExpressionRealKeyword &) -> void override;
  auto handle(st::Node &, st::ExpressionChar &) -> void override;
  auto handle(st::Node &, st::ExpressionString &) -> void override;
  auto handle(st::Node &, st::ExpressionTuple &) -> void override;
  auto handle(st::Node &, st::ExpressionUnaryOp &) -> void override;
  auto handle(st::Node &, st::ExpressionBinaryOp &) -> void override;
  auto handle(st::Node &, st::ExpressionRange &) -> void override;
  auto handle(st::Node &, st::ExpressionVector &) -> void override;
  auto handle(st::Node &, st::ExpressionArray &) -> void override;
  auto handle(st::Node &, st::ExpressionDictionary &) -> void override;
  auto handle(st::Node &, st::ExpressionAssignment &) -> void override;
  auto handle(st::Node &, st::ExpressionCall &) -> void override;
  auto handle(st::Node &, st::ExpressionLambda &) -> void override;

public:
  Compiler() {
  }

  ~Compiler() {
    free_program(&_program);
  }

  Compiler(const Compiler &) = delete;
  Compiler(Compiler &&)      = default;

  auto add_breakpoint(size_t) noexcept -> void;
  auto add_breakpoint(std::string) noexcept -> void;
  auto execute(Package &&) noexcept -> Program override;
};

} // namespace moth

} // namespace silk
