#pragma once

#include <memory>
#include <string>
#include <vector>

#include "expr.h"
#include "token.h"

namespace Stmt {

struct Empty;

struct Entrypoint;
struct Package;
struct Import;
struct Forwarding;
struct Export;

struct Variable;
struct Function;
struct Struct;
struct Constructor;
struct Destructor;

struct Loop;
struct LoopInterupt;
struct Conditional;
struct Block;
struct Return;
struct ExprStmt;

using Stmt = std::variant<
  std::unique_ptr<Empty>,

  std::unique_ptr<Entrypoint>,
  std::unique_ptr<Package>,
  std::unique_ptr<Import>,
  std::unique_ptr<Forwarding>,
  std::unique_ptr<Export>,

  std::unique_ptr<Variable>,
  std::unique_ptr<Function>,
  std::unique_ptr<Struct>,
  std::unique_ptr<Constructor>,
  std::unique_ptr<Destructor>,

  std::unique_ptr<Loop>,
  std::unique_ptr<LoopInterupt>,
  std::unique_ptr<Conditional>,
  std::unique_ptr<Block>,
  std::unique_ptr<Return>,
  std::unique_ptr<ExprStmt>>;

struct Empty {};

struct Entrypoint {};

struct Package {
  const std::string name;
  Package(const std::string& name) : name(name) {
  }
};

struct Import {
  const std::string name;
  Import(const std::string& name) : name(name) {
  }
};

struct Export {
  const Stmt decl;
  Export(Stmt&& decl) : decl(std::move(decl)) {
  }
};

struct Forwarding {
  const std::string name;
  Forwarding(const std::string& name) : name(name) {
  }
};

struct Variable {
  const std::string name;
  const Expr::Expr  init;
  const std::string type;

  Variable(const std::string& name, Expr::Expr&& init) :
      name(name), init(std::move(init)), type("") {
  }

  Variable(const std::string name, const std::string type) :
      name(name), init(), type(type) {
  }
};

struct Function {
  const std::string              name;
  const std::vector<std::string> parameters {};
  const Stmt                     body;
  const bool                     virt;

  Function(
    std::string&              name,
    std::vector<std::string>& parameters,
    Stmt&&                    body,
    bool                      virt) :
      name(name),
      parameters(parameters),
      body(std::move(body)),
      virt(virt) {
  }
};

struct Struct {
  const std::string       name;
  const std::string       parent;
  const std::vector<Stmt> body;

  Struct(std::string& name, std::string& parent, std::vector<Stmt>&& body) :
      name(name), parent(parent), body(std::move(body)) {
  }
};

struct Constructor {
  const std::vector<std::string> parameters {};
  const Stmt                     body;

  Constructor(std::vector<std::string> parameters, Stmt&& body) :
      parameters(parameters), body(std::move(body)) {
  }
};

struct Destructor {
  const Stmt body;
  Destructor(Stmt&& body) : body(std::move(body)) {
  }
};

struct Loop {
  const Expr::Expr clause;
  const Stmt       body;

  Loop(Expr::Expr&& clause, Stmt&& body) :
      clause(std::move(clause)), body(std::move(body)) {
  }
};

struct LoopInterupt {
  const bool should_continue;
  LoopInterupt(const bool& should_continue) :
      should_continue(should_continue) {
  }
};

struct Conditional {
  const Expr::Expr clause;
  const Stmt       true_stmt;
  const Stmt       false_stmt;

  Conditional(Expr::Expr&& clause, Stmt&& true_stmt, Stmt&& false_stmt) :
      clause(std::move(clause)),
      true_stmt(std::move(true_stmt)),
      false_stmt(std::move(false_stmt)) {
  }
};

struct Block {
  const std::vector<Stmt> body;
  Block(std::vector<Stmt>&& body) : body(std::move(body)) {
  }
};

struct Return {
  const Expr::Expr ret;

  Return(Expr::Expr&& ret) : ret(std::move(ret)) {
  }
};

struct ExprStmt {
  const Expr::Expr expr;

  ExprStmt(Expr::Expr&& expr) : expr(std::move(expr)) {
  }
};

template <class T>
struct Visitor {
  template <class Ptr>
  auto operator()(Ptr&& ptr) -> T {
    return this->execute(*ptr.get());
  }

  auto execute_stmt(const Stmt& stmt) -> T {
    return std::visit(*this, stmt);
  }

  virtual T execute(const Empty&)      = 0;
  virtual T execute(const Entrypoint&) = 0;
  virtual T execute(const Package&)    = 0;
  virtual T execute(const Import&)     = 0;
  virtual T execute(const Export&)     = 0;
  virtual T execute(const Forwarding&) = 0;

  virtual T execute(const Variable&)    = 0;
  virtual T execute(const Function&)    = 0;
  virtual T execute(const Struct&)      = 0;
  virtual T execute(const Constructor&) = 0;
  virtual T execute(const Destructor&)  = 0;

  virtual T execute(const Loop&)         = 0;
  virtual T execute(const LoopInterupt&) = 0;
  virtual T execute(const Conditional&)  = 0;
  virtual T execute(const Block&)        = 0;
  virtual T execute(const Return&)       = 0;
  virtual T execute(const ExprStmt&)     = 0;
};

}; // namespace Stmt
