#pragma once

#include <vector>

#include "expr.h"
#include "stmt.h"

// the AST contains the abstract representation (i.e. expressions and
// statements) of a single source file, it is the output of the parser

struct AST {
  private:
  const std::vector<Stmt::Stmt> _program;

  public:
  AST(std::vector<Stmt::Stmt>&& program) : _program(std::move(program)) {
  }

  AST(AST&) = delete;

  template <class T>
  void execute_with(Stmt::Visitor<T>& vis) {
    for (auto& stmt : _program) {
      std::visit(vis, stmt);
    }
  }
};
