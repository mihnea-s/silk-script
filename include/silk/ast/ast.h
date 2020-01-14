#pragma once

#include <vector>

#include "stmt.h"

struct AST {
  private:
  const std::vector<Stmt::Stmt> _program;

  public:
  AST(std::vector<Stmt::Stmt>&& program) : _program(std::move(program)) {
  }

  AST(AST&) = delete;

  template <class T>
  void evaluate_with(Stmt::Visitor<T>& vis) {
    for (auto& stmt : _program) {
      std::visit(vis, stmt);
    }
  }
};