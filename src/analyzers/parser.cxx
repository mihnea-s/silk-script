#include <algorithm>
#include <memory>
#include <variant>
#include <vector>

#include <silk/analyzers/parser.h>
#include <silk/ast/expr.h>
#include <silk/ast/stmt.h>
#include <silk/ast/token.h>
#include <silk/error.h>

const std::vector<std::string> Parser::restricted_names {
  "own", // this instance
  "sup", // super object
};

inline auto Parser::forward() -> void {
  _tok++;
}

inline auto Parser::backward() -> void {
  _tok--;
}

inline auto Parser::advance() -> Token {
  auto tkn = current();
  _tok++;
  return tkn;
}

inline auto Parser::current() const -> Token {
  return *_tok;
}

inline auto Parser::eof() const -> bool {
  return _tok == _end;
}

inline auto Parser::throw_error(std::string_view msg) -> void {
  throw ParsingError {Severity::error, msg, current().location()};
}

inline auto Parser::should_match(TokenType type, std::string_view msg) const
  -> void {
  if (!match(type)) {
    throw ParsingError {Severity::warning, msg, current().location()};
  }
}

inline auto Parser::must_match(TokenType type, std::string_view msg) const
  -> void {
  if (!match(type)) {
    throw ParsingError {Severity::error, msg, current().location()};
  }
}

inline auto Parser::should_consume(TokenType type, std::string_view msg)
  -> void {
  if (!consume(type)) {
    throw ParsingError {Severity::warning, msg, current().location()};
  }
}

inline auto Parser::must_consume(TokenType type, std::string_view msg)
  -> void {
  if (!consume(type)) {
    throw ParsingError {Severity::error, msg, current().location()};
  }
}

auto Parser::identifier_str() -> std::string {
  must_match(TokenType::identifier, "expected identifier");
  return advance().lexeme();
}

auto Parser::package_str() -> std::string {
  must_match(TokenType::literal_str, "expected package string");
  return advance().lexeme();
}

auto Parser::parameters() -> Stmt::Function::Parameters {
  Stmt::Function::Parameters parameters {};

  // assume at least one parameter
  auto first_param = identifier_str();
  auto first_type  = std::string {};

  // optional type specification
  if (consume(TokenType::sym_colon)) first_type = identifier_str();

  parameters.push_back({first_param, first_type});

  // as long as we're not at the closing parenthesis
  // loop for more parameters
  while (!consume(TokenType::sym_rround)) {
    // expect comma between parameters
    must_consume(TokenType::sym_comma, "expecting `,`");

    // param name and optional type
    first_param = identifier_str();
    if (consume(TokenType::sym_colon)) first_type = identifier_str();

    parameters.push_back({first_param, first_type});
  }

  return parameters;
}

auto Parser::arrow() -> Stmt::Stmt {
  // consume second part of arrow
  must_consume(TokenType::sym_gt, "expected arrow (->)");

  // get return expression
  auto ret = expr();

  // consume final semicolon
  must_consume(
    TokenType::sym_semicolon,
    "expected semicolon (;) after arrow function");

  // return statement with ret as the returned expression
  return mk_stmt<Stmt::Interrupt>(
    std::move(ret), Stmt::Interrupt::Type::ret);
}

auto Parser::expr_lambda() -> Expr::Expr {
  // if the current token is not `fct` return normal expr
  if (!consume(TokenType::kw_function)) { return expr_assignment(); }

  // parameters are optional, look for them
  // only if an opening parenthesis is found
  auto params = Stmt::Function::Parameters {};
  if (consume(TokenType::sym_lround)) { params = parameters(); }

  // expect the function body to be a block
  must_consume(TokenType::sym_lbrace, "expected `{`");
  auto body = stmt_block();

  return std::make_unique<Expr::Lambda>(params, std::move(body));
}

auto Parser::expr_assignment() -> Expr::Expr {
  // get left side of expression
  auto var = expr_logic_or();

  // if the next symbol is `=` this must be
  // an assignment expression..
  if (consume(TokenType::sym_equal)) {
    auto value = expr();

    try {
      // assignment target must be an identifier
      // this throws if it is not
      auto& id = std::get<std::unique_ptr<Expr::Identifier>>(var);

      // for brevity
      const auto beg = restricted_names.begin();
      const auto end = restricted_names.end();

      // throw error if assignment target is in restricted
      // names list
      if (std::find(beg, end, id->value) != end) {
        throw_error("invalid assignment target, cannot assign");
      }

      // return expression assignment
      return std::make_unique<Expr::Assignment>(
        id->value, std::move(value));
    } catch (std::bad_variant_access) { throw_error("invalid assignment"); }
  }

  // ..if not return the left side
  return var;
};

auto Parser::expr_logic_or() -> Expr::Expr {
  auto left = expr_logic_and();

  while (match(
    TokenType::sym_pipe, TokenType::sym_pipepipe, TokenType::sym_caret)) {
    auto operation = advance();
    auto right     = expr_logic_and();

    left = std::make_unique<Expr::Binary>(
      std::move(left), operation, std::move(right));
  }

  return left;
}

auto Parser::expr_logic_and() -> Expr::Expr {
  auto left = expr_equality();

  while (match(TokenType::sym_amp, TokenType::sym_ampamp)) {
    auto operation = advance();
    auto right     = expr_equality();

    left = std::make_unique<Expr::Binary>(
      std::move(left), operation, std::move(right));
  }

  return left;
}

auto Parser::expr_equality() -> Expr::Expr {
  auto left = expr_comparison();

  while (match(TokenType::sym_equalequal, TokenType::sym_bangequal)) {
    auto operation = advance();
    auto right     = expr_comparison();

    left = std::make_unique<Expr::Binary>(
      std::move(left), operation, std::move(right));
  }

  return left;
}

auto Parser::expr_comparison() -> Expr::Expr {
  auto left = expr_addition();

  while (match(
    TokenType::sym_gt,
    TokenType::sym_lt,
    TokenType::sym_gtequal,
    TokenType::sym_ltequal)) {
    auto operation = advance();
    auto right     = expr_addition();

    left = std::make_unique<Expr::Binary>(
      std::move(left), operation, std::move(right));
  }

  return left;
}

auto Parser::expr_addition() -> Expr::Expr {
  auto left = expr_multiplication();

  while (match(TokenType::sym_plus, TokenType::sym_minus)) {
    auto operation = advance();
    auto right     = expr_multiplication();

    left = std::make_unique<Expr::Binary>(
      std::move(left), operation, std::move(right));
  }

  return left;
}

auto Parser::expr_multiplication() -> Expr::Expr {
  auto left = expr_pow();

  while (match(
    TokenType::sym_slash,
    TokenType::sym_slashslash,
    TokenType::sym_star,
    TokenType::sym_percent)) {
    auto operation = advance();
    auto right     = expr_pow();

    left = std::make_unique<Expr::Binary>(
      std::move(left), operation, std::move(right));
  }

  return left;
}

auto Parser::expr_pow() -> Expr::Expr {
  auto left = expr_unary();

  while (match(TokenType::sym_starstar)) {
    auto operation = advance();
    auto right     = expr_unary();

    left = std::make_unique<Expr::Binary>(
      std::move(left), operation, std::move(right));
  }

  return left;
}

auto Parser::expr_unary() -> Expr::Expr {
  if (match(
        TokenType::sym_bang, TokenType::sym_tilde, TokenType::sym_minus)) {

    auto tok = advance();
    return std::make_unique<Expr::Unary>(tok, std::move(expr_unary()));
  }

  return expr_grouping();
}

auto Parser::expr_grouping() -> Expr::Expr {
  if (consume(TokenType::sym_lround)) {
    auto e = expr_logic_or();
    must_consume(TokenType::sym_rround, "expected closing ')'");
    return std::make_unique<Expr::Grouping>(std::move(e));
  }

  return expr_call();
}

auto Parser::expr_call() -> Expr::Expr {
  auto callee = expr_terminal();

  if (!match(TokenType::sym_lround, TokenType::sym_dot)) { return callee; }

  while (match(TokenType::sym_lround, TokenType::sym_dot)) {

    if (consume(TokenType::sym_lround)) {
      auto args = std::vector<Expr::Expr> {};

      while (!consume(TokenType::sym_rround)) {
        args.emplace_back(expr());

        if (consume(TokenType::sym_rround)) {
          break;
        } else {
          must_consume(
            TokenType::sym_comma, "missing `,` between call arguments");
        }
      }

      callee =
        std::make_unique<Expr::Call>(std::move(callee), std::move(args));
    } else if (consume(TokenType::sym_dot)) {
      auto property = identifier_str();
      callee = std::make_unique<Expr::Get>(std::move(callee), property);
    } else {
      throw_error("unexpected symbol");
    }
  }

  return callee;
}

auto Parser::expr_terminal() -> Expr::Expr {
  switch ((current()).type()) {
    case TokenType::literal_int: {
      return mk_expr<Expr::IntLiteral>(expr_int_literal());
    }

    case TokenType::literal_dbl: {
      return mk_expr<Expr::DoubleLiteral>(expr_dbl_literal());
    }

    case TokenType::literal_str: {
      return mk_expr<Expr::StringLiteral>(expr_str_literal());
    }

    case TokenType::identifier: {
      return mk_expr<Expr::Identifier>(expr_identifier());
    }

    case TokenType::kw_true: {
      return mk_expr<Expr::BoolLiteral>(true);
    }

    case TokenType::kw_false: {
      return mk_expr<Expr::BoolLiteral>(false);
    }

    case TokenType::kw_vid: {
      return mk_expr<Expr::Vid>();
    }

    default: {
      throw_error("expected literal or identifier");
      return mk_expr<Expr::Vid>();
    }
  }
}

auto Parser::expr_int_literal() -> Expr::IntLiteral {
  must_match(TokenType::literal_int, "expected integer literal");

  // get lexeme and convert to integer
  auto value = std::atoi(advance().lexeme().c_str());

  return Expr::IntLiteral {value};
}

auto Parser::expr_dbl_literal() -> Expr::DoubleLiteral {
  must_match(TokenType::literal_dbl, "expected double literal");

  // get lexeme and convert into double
  auto value = std::atof(advance().lexeme().c_str());

  return Expr::DoubleLiteral {value};
}

auto Parser::expr_str_literal() -> Expr::StringLiteral {
  must_match(TokenType::literal_str, "expected string literal");

  // return 'raw' lexeme
  return Expr::StringLiteral {advance().lexeme()};
}

auto Parser::expr_identifier() -> Expr::Identifier {
  must_match(TokenType::identifier, "expected identifier");

  // return lexeme as the identifier
  return Expr::Identifier {advance().lexeme()};
}

auto Parser::stmt_main() -> Stmt::Stmt {
  must_consume(TokenType::sym_semicolon, "expected `;` after main");
  return mk_stmt<Stmt::Main>();
}

auto Parser::stmt_package() -> Stmt::Stmt {
  // get package name
  auto pkg = package_str();

  // expect semicolon
  must_consume(TokenType::sym_semicolon, "expected `;` after package");

  return mk_stmt<Stmt::Package>(pkg);
}

auto Parser::stmt_import() -> Stmt::Stmt {
  // get imported package
  auto pkg = package_str();

  // expect semicolon
  must_consume(TokenType::sym_semicolon, "expected `;` after import");

  return mk_stmt<Stmt::Import>(pkg);
}

auto Parser::stmt_if() -> Stmt::Stmt {
  // opening parenthesis
  must_consume(TokenType::sym_lround, "expected `(` after if");

  // the clause expression
  auto clause = expr();

  // closing parenthesis
  must_consume(TokenType::sym_rround, "expected `)` after if clause");

  // required true statement
  auto true_stmt = stmt();

  // false statement is by default empty
  auto false_stmt = mk_stmt<Stmt::Empty>();

  // optional false statement
  if (consume(TokenType::kw_else)) { false_stmt = stmt(); }

  return mk_stmt<Stmt::Conditional>(
    std::move(clause), std::move(true_stmt), std::move(false_stmt));
}

auto Parser::stmt_for() -> Stmt::Stmt {
  auto init    = mk_stmt<Stmt::Empty>();
  auto counter = mk_expr<Expr::Vid>();

  // clause is by default constantly true (useful for infinite loops)
  auto clause = mk_expr<Expr::BoolLiteral>(true);

  // for loop arguments
  if (consume(TokenType::sym_lround)) {

    // if the first argument if a variable declaration
    if (match(TokenType::kw_let)) {

      // get the declaration statement (note: statements are always followed
      // by a `;` so we do not have to check for it)
      init = decl_variable();

      // get the required clause expression
      clause = expr();

      // optional counter
      if (consume(TokenType::sym_semicolon)) { counter = expr(); }

    } else {

      // if it is a simple while loop
      clause = expr();
    }

    must_consume(TokenType::sym_rround, "expected `)`");
  }

  // for loop body
  auto body = stmt();

  // add the counter statement to the end of the for loop iteration
  // by wrapping it in a block statement with the body
  auto blk = std::vector<Stmt::Stmt> {};
  blk.push_back(std::move(body));
  blk.push_back(mk_stmt<Stmt::ExprStmt>(std::move(counter)));

  // body becomes the block with counter
  body = mk_stmt<Stmt::Block>(std::move(blk));

  // restore vector to usable state after move
  blk.clear();

  // make the loop with the clause and body
  auto loop = mk_stmt<Stmt::Loop>(std::move(clause), std::move(body));

  // enclose the init statement and the loop in a block
  blk.push_back(std::move(init));
  blk.push_back(std::move(loop));
  return mk_stmt<Stmt::Block>(std::move(blk));
}

auto Parser::stmt_interrupt() -> Stmt::Stmt {
  // for brevity
  using InterruptType = Stmt::Interrupt::Type;

  switch (current().type()) {

    // break statement
    case TokenType::kw_break: {
      return mk_stmt<Stmt::Interrupt>(
        mk_expr<Expr::Vid>(), InterruptType::brk);
    }

    // continue statement
    case TokenType::kw_continue: {
      return mk_stmt<Stmt::Interrupt>(
        mk_expr<Expr::Vid>(), InterruptType::brk);
    }

    // invalid case
    default: {
      throw_error("expected `break` or `continue`");
      return mk_stmt<Stmt::Empty>();
    }
  }
}

auto Parser::stmt_return() -> Stmt::Stmt {
  // default return is vid
  auto ret = mk_expr<Expr::Vid>();

  // optional return value
  if (!match(TokenType::sym_semicolon)) { ret = expr(); }

  // semicolon after return
  must_consume(TokenType::sym_semicolon, "expected `;` after return");

  return mk_stmt<Stmt::Interrupt>(
    std::move(ret), Stmt::Interrupt::Type::ret);
}

auto Parser::stmt_block() -> Stmt::Stmt {
  // block body
  std::vector<Stmt::Stmt> body {};

  // add statements to block until we hit a closing
  // brace (`}`) or the end of the file
  while (!eof() && !match(TokenType::sym_rbrace)) {
    body.emplace_back(decl());
  }

  // if we hit the end of the file
  must_consume(TokenType::sym_rbrace, "expected `}` after block");

  return mk_stmt<Stmt::Block>(std::move(body));
}

auto Parser::stmt_exprstmt() -> Stmt::Stmt {
  // expression
  auto inner = expr();

  // required end of statement semicolon
  must_consume(TokenType::sym_semicolon, "expected `;` after statement");

  return mk_stmt<Stmt::ExprStmt>(std::move(inner));
}

auto Parser::decl_variable() -> Stmt::Stmt {
  must_consume(TokenType::kw_let, "expected keyword `let`");

  // name of the variable
  auto name = identifier_str();

  // for brevity
  const auto beg = restricted_names.begin();
  const auto end = restricted_names.end();

  // cannot declare var with restricted names (e.g. own)
  if (std::find(beg, end, name) != end) {
    throw_error("invalid variable name, it is a restricted name");
  }

  // type of the variable is by default dynamic
  auto type = std::string {};

  // initialization statement is vid by default
  auto init = mk_expr<Expr::Vid>();

  // if the user specified a type
  if (consume(TokenType::sym_colon)) { type = identifier_str(); }

  // if the user specified an init expression
  if (consume(TokenType::sym_equal)) { init = expr(); }

  // ending semicolon;
  must_consume(TokenType::sym_semicolon, "expected `;`");

  return mk_stmt<Stmt::Variable>(name, type, std::move(init));
}

auto Parser::decl_function() -> Stmt::Stmt {
  must_consume(TokenType::kw_function, "expected `fct` keyword");

  // if there is no name we assume this was actually an expression
  // whose foremost operand is a lambda
  if (!match(TokenType::identifier)) {
    backward();
    return stmt_exprstmt();
  }

  // the name of the declared function
  auto name = identifier_str();

  // empty parameters by default
  auto params = Stmt::Function::Parameters {};

  // empty body for virtual functions
  auto body = mk_stmt<Stmt::Empty>();

  // assume normal function at first
  auto type = Stmt::Function::Type::fct;

  // handle optional parameters
  if (consume(TokenType::sym_lround)) { params = parameters(); }

  // handle function type
  switch (advance().type()) {

    // normal function with block body
    case TokenType::sym_lbrace: {
      body = stmt_block();
      break;
    }

    // arrow function
    case TokenType::sym_minus: {
      body = arrow();
      break;
    }

    // virtual function with no body
    case TokenType::kw_virt: {
      must_consume(TokenType::sym_semicolon, "expected `;` after virt fct");
      type = Stmt::Function::Type::virt;
      break;
    }

    // invalid case
    default: {
      throw_error("unexpected symbol after function declaration");
    }
  }

  return mk_stmt<Stmt::Function>(name, params, std::move(body), type);
}

auto Parser::decl_struct() -> Stmt::Stmt {
  must_consume(TokenType::kw_struct, "expected keyword `struct`");

  auto name   = identifier_str();
  auto parent = std::string {};

  // expect constructor and destructor
  // to not be defined at first
  auto ctor = mk_expr<Expr::Vid>();
  auto dtor = mk_expr<Expr::Vid>();

  auto fields  = std::vector<Stmt::Stmt> {};
  auto methods = std::vector<Stmt::Stmt> {};

  // handle optional parent
  if (consume(TokenType::sym_slash)) { parent = identifier_str(); }

  // struct body must start with opening brace `{`
  must_consume(TokenType::sym_lbrace, "missing struct body");

  // handle fields
  while (match(TokenType::kw_let)) {
    fields.emplace_back(decl_variable());
  }

  // expect construct and destructor definitions
  // after struct fields
  if (consume(TokenType::kw_ctor)) {
    // assume no parameters
    auto params = Stmt::Function::Parameters {};

    // optional parameters
    if (consume(TokenType::sym_lround)) { params = parameters(); }

    // ctor body
    must_consume(TokenType::sym_lbrace, "expected `{` after `ctor`");
    auto body = stmt_block();

    ctor = mk_expr<Expr::Lambda>(params, std::move(body));
  }

  if (consume(TokenType::kw_dtor)) {
    // dtor body
    must_consume(TokenType::sym_lbrace, "expected `{` after `dtor`");
    auto body = stmt_block();

    // no parameters
    auto params = Stmt::Function::Parameters {};

    dtor = mk_expr<Expr::Lambda>(params, std::move(body));
  }

  // handle methods
  while (match(TokenType::kw_function)) {
    methods.emplace_back(decl_function());
  }

  // struct body must end with closing brace `}`
  must_consume(TokenType::sym_rbrace, "expected `}` after struct body");

  return mk_stmt<Stmt::Struct>(
    name,
    parent,
    std::move(ctor),
    std::move(dtor),
    std::move(fields),
    std::move(methods));
}

auto Parser::expr() -> Expr::Expr {
  return expr_lambda();
}

auto Parser::stmt() -> Stmt::Stmt {
  switch (advance().type()) {
    case TokenType::kw_main: return stmt_main();
    case TokenType::kw_package: return stmt_package();
    case TokenType::kw_import: return stmt_import();
    case TokenType::kw_if: return stmt_if();
    case TokenType::kw_for: return stmt_for();
    case TokenType::kw_return: return stmt_return();
    case TokenType::sym_lbrace: return stmt_block();

    case TokenType::kw_break: {
      backward();
      return stmt_interrupt();
    }

    case TokenType::kw_continue: {
      backward();
      return stmt_interrupt();
    }

    // if no keywords match assume this is an
    // expression statement
    default: {
      backward();
      return stmt_exprstmt();
    }
  }
}

auto Parser::decl() -> Stmt::Stmt {
  switch (current().type()) {
    case TokenType::kw_let: return decl_variable();
    case TokenType::kw_function: return decl_function();
    case TokenType::kw_struct: return decl_struct();

    // if no keywords match assume this is a
    // statement instead
    default: return stmt();
  }
}

auto Parser::program() -> std::vector<Stmt::Stmt> {
  std::vector<Stmt::Stmt> stmts {};

  // parse statements until end of file
  while (!eof()) {
    try {
      stmts.emplace_back(decl());
    } catch (ParsingError e) {
      // if a parsing error occurs add it to the vector..
      _errors.push_back(e);

      // ..and advance past the next semicolon or end of file,
      // not perfect but it does the job
      while (!eof() && !consume(TokenType::sym_semicolon)) {
        advance();
      }
    }
  }

  return std::move(stmts);
}

// error getters

auto Parser::has_error() const -> bool {
  return !_errors.empty();
}

auto Parser::errors() const -> const std::vector<ParsingError>& {
  return _errors;
}

// public parsing interface

auto Parser::parse(Iter begin, Iter end) noexcept -> AST {
  _tok = begin;
  _end = end;
  return AST {std::move(program())};
}
