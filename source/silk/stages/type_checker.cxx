#include <silk/stages/type_checker.h>

namespace silk {

auto TypeChecker::handle(st::Node &, st::Comment &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ModuleMain &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ModuleDeclaration &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ModuleImport &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::DeclarationFunction &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::DeclarationEnum &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::DeclarationObject &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::DeclarationDynamicLibrary &)
  -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::DeclarationMacro &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::StatementEmpty &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::StatementExpression &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::StatementBlock &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::StatementCircuit &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::StatementVariable &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::StatementConstant &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::StatementReturn &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::StatementSwitch &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::StatementIterationControl &)
  -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::StatementIf &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::StatementWhile &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::StatementLoop &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::StatementFor &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::StatementForeach &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::StatementMatch &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ExpressionIdentifier &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ExpressionVoid &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &node, st::ExpressionContinuation &data)
  -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ExpressionBool &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ExpressionNat &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ExpressionInt &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ExpressionReal &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ExpressionRealKeyword &)
  -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ExpressionChar &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ExpressionString &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ExpressionTuple &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ExpressionUnaryOp &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ExpressionBinaryOp &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ExpressionRange &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ExpressionVector &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ExpressionArray &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ExpressionDictionary &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ExpressionAssignment &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ExpressionCall &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::handle(st::Node &, st::ExpressionLambda &) -> st::Typing {
  return nullptr;
}

auto TypeChecker::execute(Module &&mod) noexcept -> Module {
  return std::move(mod);
}

} // namespace silk
