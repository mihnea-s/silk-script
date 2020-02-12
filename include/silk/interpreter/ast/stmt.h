#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../../common/token.h"
#include "expr_decl.h"
#include "stmt_decl.h"

namespace Stmt {

// the empty statement
struct Empty {};

// statement marking the file as main
struct Main {};

// statement marking the file as part
// of a package (e.g. 'pkg 'mypkg';')
struct Package {
  const std::string name;
  Package(const std::string& name) : name(name) {
  }
};

// an import statement of an external
// or standard library package
struct Import {
  const std::string name;
  Import(const std::string& name) : name(name) {
  }
};

// a variable declaration
struct Variable {
  const std::string name;
  const std::string type;
  const Expr::Expr  init;

  Variable(const std::string& name, std::string type, Expr::Expr&& init) :
      name(name), type(type), init(std::move(init)) {
  }
};

// a function declaration
// (note: if the function is virtual the
// body will be an empty statement
//  otherwise it will be a block statement)
struct Function {
  using Parameters = std::vector<std::pair<std::string, std::string>>;
  enum class Type {
    fct,
    virt,
  };

  const std::string name;
  const Parameters  parameters {};
  const Stmt        body;
  const Type        type;

  Function(std::string& name, Parameters& parameters, Stmt&& body, Type type) :
      name(name), parameters(parameters), body(std::move(body)), type(type) {
  }
};

// a struct declaration
// (note: if they are not explicitly declared
// the constructor and destructor will be empty
// statements)
// (note: parent is an empty string if the struct
// doesn't inherit from anything)
struct Struct {
  const std::string name;
  const std::string parent;

  const Expr::Expr ctor;
  const Expr::Expr dtor;

  const std::vector<Stmt> fields;
  const std::vector<Stmt> methods;

  Struct(
    std::string& name,
    std::string& parent,

    Expr::Expr&& ctor,
    Expr::Expr&& dtor,

    std::vector<Stmt>&& fields,
    std::vector<Stmt>&& methods) :
      name(name),
      parent(parent),

      ctor(std::move(ctor)),
      dtor(std::move(dtor)),

      fields(std::move(fields)),
      methods(std::move(methods)) {
  }
};

// a simple while loop
struct Loop {
  const Expr::Expr clause;
  const Stmt       body;

  Loop(Expr::Expr&& clause, Stmt&& body) :
      clause(std::move(clause)), body(std::move(body)) {
  }
};

// an if expression with an optional
// else statement
// (note: false_stmt will be an empty
// statement if else is omitted)
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

// a wrapper around a new scope surrounded
// by braces (e.g. '{ let x = 2; }')
struct Block {
  const std::vector<Stmt> body;
  Block(std::vector<Stmt>&& body) : body(std::move(body)) {
  }
};

// Interrupt is used for break, continue or return,
// it holds the type of interrupt and, in the case of
// a return, the returned value
struct Interrupt {
  enum class Type {
    ret,
    brk,
    cont,
  };

  const Expr::Expr ret;
  const Type       type;

  Interrupt(Expr::Expr&& ret, Type type) : ret(std::move(ret)), type(type) {
  }
};

// an expression wrapped in a statement
// (e.g. a function call 'myFunction();')
struct ExprStmt {
  const Expr::Expr expr;

  ExprStmt(Expr::Expr&& expr) : expr(std::move(expr)) {
  }
};

// visitor pattern boilerplate
// classes used to execute statements inherit from this
template <class T>
struct Visitor {

  // since variant uses unique pointers we
  // need a wrapper that will automatically
  // dereference the pointer and call the
  // appropriate function
  template <class Ptr>
  auto operator()(Ptr&& ptr) -> T {
    return this->execute(*ptr.get());
  }

  // helper method to execute statements
  auto execute_stmt(const Stmt& stmt) -> T {
    return std::visit(*this, stmt);
  }

  // visitor pattern boilerplate (visit functions)

  virtual T execute(const Empty&)   = 0;
  virtual T execute(const Main&)    = 0;
  virtual T execute(const Package&) = 0;

  virtual T execute(const Import&) = 0;

  virtual T execute(const Variable&) = 0;
  virtual T execute(const Function&) = 0;
  virtual T execute(const Struct&)   = 0;

  virtual T execute(const Loop&)        = 0;
  virtual T execute(const Conditional&) = 0;
  virtual T execute(const Block&)       = 0;
  virtual T execute(const Interrupt&)   = 0;
  virtual T execute(const ExprStmt&)    = 0;
};

}; // namespace Stmt
