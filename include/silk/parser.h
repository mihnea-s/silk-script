#pragma once

#include <cstddef>
#include <cstdlib>
#include <exception>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <string_view>
#include <variant>
#include <vector>

#include "ast/ast.h"
#include "ast/expr.h"
#include "ast/stmt.h"
#include "ast/token.h"
#include "error.h"

/* silk scripts's grammar

program        → ( decl )* eof

decl           → variable | function | struct | stmt

variable       → `let` identifier (( `:` identifier ) | ( `=` expr )) ;
function       → `fct` identifier parameters? ( `virt` | block ; )
struct         → `struct` identifier ( `/` identifier )? block
ctor           → `ctor` parameters? block
dtor           → `dtor` block
export         → `export` ( variable | function | struct )

stmt           → package | entrypoint | import | exportimport | assignment |
                 ifstmt | forstmt | block | return | exprstmt

package        → `pkg` literal ;
entrypoint     → `entrypoint` ;
import         → `use` literal ;
forwarding     → `with` literal ;
return         → `return` expr? ;
interupt       → ( `break` | `continue` ) ;
assignment     → identfier `=` expr ;
ifstmt         → `if` `(` expr `)` stmt ( `else` stmt )?
forstmt        → `for` `(` ( variable | expr ) expr? `;` expr? `)` stmt
block          → `{` decl* `}`
exprstmt       → expr ;

expr           → assignment
assignment     → identifier `=` expr | logic_or
logic_or       → logic_and ( (`||` | `|` | '^') logic_and )*
logic_and      → equality ( (`&&` | `&`) equality )*
equality       → comparison ( ( `!=` | `==` ) comparison )*
comparison     → addition ( ( `>` | `>=` | `<` | `<=` ) addition )*
addition       → multiplication ( ( `-` | `+` ) multiplication )*
multiplication → pow ( ( `/` | `*` | `//` | `%` ) pow )*
pow            → unary ( ( `**` ) unary )*
unary          → ( `!` | `-` | `~` ) unary | grouping
grouping       → `(` expr `)` | call
call           → boolean ( `(` arguments? `)` )*
boolean        → `true` | `false` | terminal
terminal       → literal | identifier | `vid`

arguments      → expression ( `,` expression )*
parameters     → `(` identifier ( `,` identfier )* `)`


// TODO:
// - export
// - super
// - this

*/

struct Parser {
  private:
  using Iter = std::vector<Token>::const_iterator;

  Iter tok;
  Iter end;

  inline auto forward() -> void;
  inline auto backward() -> void;

  inline auto advance() -> Token;

  inline auto eof() const -> bool;

  inline auto empty_expr() -> Expr::Expr;
  inline auto empty_stmt() -> Stmt::Stmt;

  inline auto is_empty(Expr::Expr&) -> bool;
  inline auto is_empty(Stmt::Stmt&) -> bool;

  inline auto throw_error(Severity, std::string_view) -> void;

  template <class... Args>
  inline auto match(Args... args) const -> bool {
    if (eof()) return false;

    for (auto type : std::vector {args...}) {
      if ((*tok).type() == type) return true;
    }

    return false;
  };

  template <class... Args>
  inline auto consume(Args... args) -> bool {
    if (match(args...)) {
      advance();
      return true;
    }

    return false;
  }

  inline auto should_match(TokenType, std::string_view) const -> void;
  inline auto must_match(TokenType, std::string_view) const -> void;

  inline auto should_consume(TokenType, std::string_view) -> void;
  inline auto must_consume(TokenType, std::string_view) -> void;

  auto package_str() -> std::string;
  auto identifier_str() -> std::string;
  auto parameters_str() -> std::vector<std::string>;

  auto expr_assignment() -> Expr::Expr;
  auto expr_logic_or() -> Expr::Expr;
  auto expr_logic_and() -> Expr::Expr;
  auto expr_equality() -> Expr::Expr;
  auto expr_comparison() -> Expr::Expr;
  auto expr_addition() -> Expr::Expr;
  auto expr_multiplication() -> Expr::Expr;
  auto expr_pow() -> Expr::Expr;
  auto expr_unary() -> Expr::Expr;
  auto expr_grouping() -> Expr::Expr;
  auto expr_call() -> Expr::Expr;
  auto expr_terminal() -> Expr::Expr;

  auto expr_kw_literal() -> Expr::Expr;
  auto expr_lambda_literal() -> Expr::Expr;

  auto expr_int_literal() -> Expr::IntLiteral;
  auto expr_dbl_literal() -> Expr::DoubleLiteral;
  auto expr_str_literal() -> Expr::StringLiteral;
  auto expr_identifier() -> Expr::Identifier;

  auto stmt_entrypoint() -> Stmt::Stmt;
  auto stmt_package() -> Stmt::Stmt;
  auto stmt_import() -> Stmt::Stmt;
  auto stmt_forwarding() -> Stmt::Stmt;
  auto stmt_if() -> Stmt::Stmt;
  auto stmt_for() -> Stmt::Stmt;
  auto stmt_break() -> Stmt::Stmt;
  auto stmt_continue() -> Stmt::Stmt;
  auto stmt_block() -> Stmt::Stmt;
  auto stmt_return() -> Stmt::Stmt;
  auto stmt_exprstmt() -> Stmt::Stmt;

  auto decl_variable() -> Stmt::Stmt;
  auto decl_function() -> Stmt::Stmt;
  auto decl_struct() -> Stmt::Stmt;
  auto decl_ctor() -> Stmt::Stmt;
  auto decl_dtor() -> Stmt::Stmt;
  auto decl_export() -> Stmt::Stmt;

  auto expr() -> Expr::Expr;
  auto stmt() -> Stmt::Stmt;
  auto decl() -> Stmt::Stmt;

  auto program() -> std::vector<Stmt::Stmt>;

  public:
  auto parse(Iter begin, Iter end) noexcept -> AST;

  std::function<void(ParsingError&)> _error_callback;
  auto set_error_callback(std::function<void(ParsingError&)> cb) -> void {
    _error_callback = cb;
  }
};