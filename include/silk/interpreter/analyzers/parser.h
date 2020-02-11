#pragma once

#include <string_view>
#include <vector>

#include "../../common/error.h"
#include "../../common/token.h"
#include "../ast/ast.h"
#include "../ast/expr.h"
#include "../ast/stmt.h"

/* silk scripts's grammar

program        -> ( decl )* eof

// declarations
decl           -> variable | function | struct | stmt

  variable       -> `let` identifier `:` identifier ( `=` expr )? `;`
  function       -> `fct` identifier parameters? function_body
  struct         -> `struct` identifier ( `/` identifier )? struct_body


// statement rules
stmt           -> package | main | import | return | interrupt
                 | if | for | block | exprstmt

  package        -> `pkg` literal `;`
  main           -> `main` `;`
  import         -> `use` literal `;`
  return         -> `return` expr? `;`
  interrupt      -> ( `break` | `continue` ) `;`
  if             -> `if` `(` expr `)` stmt ( `else` stmt )?
  for            -> `for` `(` ( variable | expr ) expr? `;` expr? `)` stmt
  block          -> `{` decl* `}`
  exprstmt       -> expr `;`


// expression rules
expr           -> assignment

  lambda         -> `fct` parameters? block
  assignment     -> identifier `=` expr | logic_or
  logic_or       -> logic_and ( (`||` | `|` | '^') logic_and )*
  logic_and      -> equality ( (`&&` | `&`) equality )*
  equality       -> comparison ( ( `!=` | `==` ) comparison )*
  comparison     -> addition ( ( `>` | `>=` | `<` | `<=` ) addition )*
  addition       -> multiplication ( ( `-` | `+` ) multiplication )*
  multiplication -> pow ( ( `/` | `*` | `//` | `%` ) pow )*
  pow            -> unary ( ( `**` ) unary )*
  unary          -> ( `!` | `-` | `~` ) unary | grouping
  grouping       -> `(` expr `)` | call
  call           -> boolean ( `.` identifier | `(` arguments? `)` )*
  boolean        -> `true` | `false` | terminal
  terminal       -> literal | identifier | lambda | `true` | `false` | `vid`



// helper rules
struct_body   -> `{` ( variable | function | ctor | dtor )* `}`
ctor          -> `ctor` parameters? block
dtor          -> `dtor` block
function_body -> ( `virt` `;` | block | arrow )
arrow         -> `-` `>` expr ;
parameters    -> `(` parameter ( `,` parameter )* `)`
parameter     -> identifier `:` identifier
arguments     -> expression ( `,` expression )*

*/

struct Parser {
  private:
  using Iter = std::vector<Token>::const_iterator;

  static const std::vector<std::string> restricted_names;

  Iter _tok;
  Iter _end;

  std::vector<ParsingError> _errors;

  // position helper functions
  inline auto forward() -> void;
  inline auto backward() -> void;

  // move forward and return previous token
  inline auto advance() -> Token;

  // return current token
  inline auto previous() const -> Token;
  inline auto current() const -> Token;

  // check end of token stream
  inline auto eof() const -> bool;

  // wrapper around make_unique to transform a T (Expr::*)
  // into an Expr::Expr (variant of Expr::* unique ptrs)
  template <class T, class... Args>
  inline auto mk_expr(Args&&... args) const -> Expr::Expr {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }

  // wrapper around make_unique to transform a T (Stmt::*)
  // into a Stmt::Stmt (variant of Stmt::* unique ptrs)
  template <class T, class... Args>
  inline auto mk_stmt(Args&&... args) const -> Stmt::Stmt {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }

  // error helper
  inline auto throw_error(std::string) -> void;

  // match function return true if the next
  // token is equal to one of their arguments
  // false otherwise
  template <class... Args>
  inline auto match(Args... args) const -> bool {
    if (eof()) return false;

    for (auto type : std::vector {args...}) {
      if ((*_tok).type() == type) return true;
    }

    return false;
  };

  // wrapper around match that adds a warning if match returned false
  inline auto should_match(TokenType, std::string) const -> void;

  // wrapper around match that adds an error if match returned false
  inline auto must_match(TokenType, std::string) const -> void;

  // consume function return true *and move forward* if
  // the current token is equal to one of their arguments
  // they return false otherwise
  template <class... Args>
  inline auto consume(Args... args) -> bool {
    if (match(args...)) {
      advance();
      return true;
    }

    return false;
  }

  // see should_match & must_match
  inline auto should_consume(TokenType, std::string) -> void;
  inline auto must_consume(TokenType, std::string) -> void;

  // consume the next token if it is a(n) x
  // and return the string it contains, error otherwise
  auto identifier_str() -> std::string; // where x = identifier
  auto package_str() -> std::string;    // where x = string literal

  // matches the rule 'parameters'
  auto parameters() -> Stmt::Function::Parameters;

  // matches the rule 'arrow'
  auto arrow() -> Stmt::Stmt;

  auto expr_lambda() -> Expr::Expr;         // matches 'lambda'
  auto expr_assignment() -> Expr::Expr;     // matches 'assignment'
  auto expr_logic_or() -> Expr::Expr;       // matches 'logic_or'
  auto expr_logic_and() -> Expr::Expr;      // matches 'logic_and'
  auto expr_equality() -> Expr::Expr;       // matches 'equality'
  auto expr_comparison() -> Expr::Expr;     // matches 'comparison'
  auto expr_addition() -> Expr::Expr;       // matches 'addition'
  auto expr_multiplication() -> Expr::Expr; // matches 'multiplication'
  auto expr_pow() -> Expr::Expr;            // matches 'pow'
  auto expr_unary() -> Expr::Expr;          // matches 'unary'
  auto expr_grouping() -> Expr::Expr;       // matches 'grouping'
  auto expr_call() -> Expr::Expr;           // matches 'call'
  auto expr_terminal() -> Expr::Expr;       // matches 'terminal'

  // helper functions for literals
  // (note: they return concrete Expr::* types
  // and not Expr::Expr which is a variant of
  // unique_ptrs this is needed by *_str functions)
  auto expr_int_literal() -> Expr::IntLiteral;
  auto expr_dbl_literal() -> Expr::DoubleLiteral;
  auto expr_str_literal() -> Expr::StringLiteral;
  auto expr_identifier() -> Expr::Identifier;

  auto stmt_main() -> Stmt::Stmt;      // matches 'main'
  auto stmt_package() -> Stmt::Stmt;   // matches 'package'
  auto stmt_import() -> Stmt::Stmt;    // matches 'import'
  auto stmt_if() -> Stmt::Stmt;        // matches 'if'
  auto stmt_for() -> Stmt::Stmt;       // matches 'for'
  auto stmt_interrupt() -> Stmt::Stmt; // matches 'interrupt'
  auto stmt_return() -> Stmt::Stmt;    // matches 'return'
  auto stmt_block() -> Stmt::Stmt;     // matches 'block'
  auto stmt_exprstmt() -> Stmt::Stmt;  // matches 'exprstmt'

  auto decl_variable() -> Stmt::Stmt; // matches 'variable'
  auto decl_function() -> Stmt::Stmt; // matches 'function'
  auto decl_struct() -> Stmt::Stmt;   // matches 'struct'

  // return the lowest precedence expression
  auto expr() -> Expr::Expr;

  // return statement based on the next token
  // (i.e. tok == `if` -> return stmt_if())
  auto stmt() -> Stmt::Stmt;

  // return declaration based on the next token
  // (i.e. tok == `let` -> return decl_variable())
  auto decl() -> Stmt::Stmt;

  // matches 'program'
  auto program() -> std::vector<Stmt::Stmt>;

  public:
  // error public methods
  auto has_error() const -> bool;
  auto clear_errors() -> void;
  auto errors() const -> const std::vector<ParsingError>&;

  // public interface for parsing
  auto parse(Iter begin, Iter end) noexcept -> AST;
};