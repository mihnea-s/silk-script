#include <memory>
#include <variant>
#include <vector>

#include <silk/ast/expr.h>
#include <silk/ast/stmt.h>
#include <silk/ast/token.h>
#include <silk/parser.h>

inline auto Parser::forward() -> void {
  tok++;
}

inline auto Parser::backward() -> void {
  tok--;
}

inline auto Parser::advance() -> Token {
  auto tkn = *tok;
  tok++;
  return tkn;
}

inline auto Parser::eof() const -> bool {
  return tok == end || (*tok).type() == TokenType::eof;
}

inline auto Parser::empty_expr() -> Expr::Expr {
  return std::make_unique<Expr::Empty>();
}

inline auto Parser::empty_stmt() -> Stmt::Stmt {
  return std::make_unique<Stmt::Empty>();
}

inline auto Parser::is_empty(Expr::Expr& ex) -> bool {
  try {
    std::get<std::unique_ptr<Expr::Empty>>(ex);
    return true;
  } catch (std::bad_variant_access) { return false; };
}

inline auto Parser::is_empty(Stmt::Stmt& ex) -> bool {
  try {
    std::get<std::unique_ptr<Stmt::Empty>>(ex);
    return true;
  } catch (std::bad_variant_access) { return false; };
}

inline auto Parser::throw_error(Severity sev, std::string_view msg)
  -> void {
  throw ParsingError {sev, msg, (*tok).location()};
}

inline auto Parser::should_match(TokenType type, std::string_view msg) const
  -> void {
  if (!match(type)) {
    throw ParsingError {Severity::warning, msg, (*tok).location()};
  }
}

inline auto Parser::must_match(TokenType type, std::string_view msg) const
  -> void {
  if (!match(type)) {
    throw ParsingError {Severity::error, msg, (*tok).location()};
  }
}

inline auto Parser::should_consume(TokenType type, std::string_view msg)
  -> void {
  if (!consume(type)) {
    throw ParsingError {Severity::warning, msg, (*tok).location()};
  }
}

inline auto Parser::must_consume(TokenType type, std::string_view msg)
  -> void {
  if (!consume(type)) {
    throw ParsingError {Severity::error, msg, (*tok).location()};
  }
}

auto Parser::parameters_str() -> std::vector<std::string> {
  std::vector<std::string> parameters {};

  parameters.push_back(identifier_str());

  while (!consume(TokenType::sym_rround)) {
    must_consume(
      TokenType::sym_comma, "expecting comma between function parameters");
    parameters.push_back(identifier_str());
  }

  return parameters;
}

auto Parser::identifier_str() -> std::string {
  if (!match(TokenType::identifier)) {
    throw_error(Severity::error, "expected identifier");
  }

  auto token = advance();
  return token.lexeme();
}

auto Parser::package_str() -> std::string {
  if (!match(TokenType::literal_str)) {
    throw_error(Severity::error, "expected string literal");
  }

  auto token = advance();
  return token.lexeme();
}

auto Parser::expr_assignment() -> Expr::Expr {
  auto var = expr_logic_or();

  if (consume(TokenType::sym_equal)) {
    auto value = expr();

    try {
      auto& id = std::get<std::unique_ptr<Expr::Identifier>>(var);
      return std::make_unique<Expr::Assignment>(
        id->value, std::move(value));
    } catch (std::bad_variant_access) {
      throw_error(Severity::error, "invalid assignment");
    }
  }

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

    return std::make_unique<Expr::Unary>(
      advance(), std::move(expr_unary()));
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

  if (!match(TokenType::sym_lround)) { return callee; }

  while (consume(TokenType::sym_lround)) {
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
  }

  return callee;
}

auto Parser::expr_terminal() -> Expr::Expr {
  if (match(
        TokenType::kw_true,
        TokenType::kw_false,
        TokenType::kw_vid,
        TokenType::kw_function)) {
    return expr_kw_literal();
  }

  switch ((*tok).type()) {
    case TokenType::literal_int: {
      return std::make_unique<Expr::IntLiteral>(expr_int_literal());
    }

    case TokenType::literal_dbl: {
      return std::make_unique<Expr::DoubleLiteral>(expr_dbl_literal());
    }

    case TokenType::literal_str: {
      return std::make_unique<Expr::StringLiteral>(expr_str_literal());
    }

    case TokenType::identifier: {
      return std::make_unique<Expr::Identifier>(expr_identifier());
    }

    default: {
      throw_error(Severity::error, "expected literal or identifier");
      return empty_expr();
    }
  }
}

auto Parser::expr_kw_literal() -> Expr::Expr {
  switch (advance().type()) {
    case TokenType::kw_true: {
      return std::make_unique<Expr::BoolLiteral>(true);
    }

    case TokenType::kw_false: {
      return std::make_unique<Expr::BoolLiteral>(false);
    }

    case TokenType::kw_vid: {
      return std::make_unique<Expr::VidLiteral>();
    }

    case TokenType::kw_function: {
      return expr_lambda_literal();
    }

    default: {
      throw_error(Severity::error, "expected true, false, or vid");
      return empty_expr();
    }
  }
}

auto Parser::expr_lambda_literal() -> Expr::Expr {
  auto parameters = std::vector<std::string> {};

  if (consume(TokenType::sym_lround)) { parameters = parameters_str(); }

  must_consume(TokenType::sym_lbrace, "missing `{`");

  auto body = stmt_block();

  return std::make_unique<Expr::Lambda>(parameters, std::move(body));
}

auto Parser::expr_int_literal() -> Expr::IntLiteral {
  must_match(TokenType::literal_int, "expected integer literal");

  auto token = advance();
  auto value = std::atoi(token.lexeme().c_str());
  return Expr::IntLiteral {value};
}

auto Parser::expr_dbl_literal() -> Expr::DoubleLiteral {
  if (!match(TokenType::literal_dbl)) {
    throw_error(Severity::error, "expected double literal");
  }

  auto token = advance();
  auto value = std::atof(token.lexeme().c_str());
  return Expr::DoubleLiteral {value};
}

auto Parser::expr_str_literal() -> Expr::StringLiteral {

  if (!match(TokenType::literal_str)) {
    throw_error(Severity::error, "expected string literal");
  }

  auto token = advance();
  return Expr::StringLiteral {token.lexeme()};
}

auto Parser::expr_identifier() -> Expr::Identifier {
  if (!match(TokenType::identifier)) {
    throw_error(Severity::error, "expected identifier");
  }

  auto token = advance();
  return Expr::Identifier {token.lexeme()};
}

auto Parser::stmt_entrypoint() -> Stmt::Stmt {
  must_consume(TokenType::sym_semicolon, "expected `;` after entrypoint");
  return std::make_unique<Stmt::Entrypoint>();
}

auto Parser::stmt_package() -> Stmt::Stmt {
  auto pkg = package_str();
  must_consume(TokenType::sym_semicolon, "expected `;` after package");
  return std::make_unique<Stmt::Package>(pkg);
}

auto Parser::stmt_import() -> Stmt::Stmt {
  auto pkg = package_str();
  must_consume(TokenType::sym_semicolon, "expected `;` after import");
  return std::make_unique<Stmt::Import>(pkg);
}

auto Parser::stmt_forwarding() -> Stmt::Stmt {
  auto pkg = package_str();
  must_consume(TokenType::sym_semicolon, "expected `;` after import");
  return std::make_unique<Stmt::Forwarding>(pkg);
}

auto Parser::stmt_if() -> Stmt::Stmt {
  must_consume(TokenType::sym_lround, "expected `(` after if");

  auto clause = expr();

  must_consume(TokenType::sym_rround, "expected `)` after if clause");

  auto true_stmt  = stmt();
  auto false_stmt = empty_stmt();

  if (consume(TokenType::kw_else)) { false_stmt = stmt(); }

  return std::make_unique<Stmt::Conditional>(
    std::move(clause), std::move(true_stmt), std::move(false_stmt));
}

auto Parser::stmt_for() -> Stmt::Stmt {
  auto init    = empty_stmt();
  auto counter = empty_expr();
  auto clause  = empty_expr();

  // TODO refactor this
  if (consume(TokenType::sym_lround)) {
    if (consume(TokenType::kw_let)) {

      init   = decl_variable();
      clause = expr();

      if (consume(TokenType::sym_semicolon)) { counter = expr(); }

    } else {
      clause = expr();
    }

    must_consume(TokenType::sym_rround, "expected `)`");

  } else {
    clause = std::make_unique<Expr::BoolLiteral>(true);
  }

  auto body = stmt();

  if (!is_empty(counter)) {
    std::vector<Stmt::Stmt> blk;
    blk.push_back(std::move(body));
    blk.push_back(std::make_unique<Stmt::ExprStmt>(std::move(counter)));

    body = std::make_unique<Stmt::Block>(std::move(blk));
  }

  auto loop = empty_stmt();
  loop = std::make_unique<Stmt::Loop>(std::move(clause), std::move(body));

  if (!is_empty(init)) {
    std::vector<Stmt::Stmt> blk;
    blk.push_back(std::move(init));
    blk.push_back(std::move(loop));

    loop = std::make_unique<Stmt::Block>(std::move(blk));
  }

  return loop;
}

auto Parser::stmt_break() -> Stmt::Stmt {
  return std::make_unique<Stmt::LoopInterupt>(false);
}

auto Parser::stmt_continue() -> Stmt::Stmt {
  return std::make_unique<Stmt::LoopInterupt>(true);
}

auto Parser::stmt_block() -> Stmt::Stmt {
  std::vector<Stmt::Stmt> body {};

  while (!eof() && !match(TokenType::sym_rbrace)) {
    body.emplace_back(decl());
  }

  must_consume(TokenType::sym_rbrace, "expected `}` after block");

  return std::make_unique<Stmt::Block>(std::move(body));
}

auto Parser::stmt_return() -> Stmt::Stmt {
  auto ret = empty_expr();
  if (!match(TokenType::sym_semicolon)) { ret = expr(); }
  must_consume(TokenType::sym_semicolon, "expected `;` after return");
  return std::make_unique<Stmt::Return>(std::move(ret));
}

auto Parser::stmt_exprstmt() -> Stmt::Stmt {
  auto inner = expr();
  must_consume(TokenType::sym_semicolon, "expected `;` after statement");
  return std::make_unique<Stmt::ExprStmt>(std::move(inner));
}

auto Parser::decl_variable() -> Stmt::Stmt {

  auto name = identifier_str();

  switch (advance().type()) {

    case TokenType::sym_equal: {
      auto init = expr();

      must_consume(
        TokenType::sym_semicolon,
        "expected `;` after variable initialization");

      return std::make_unique<Stmt::Variable>(name, std::move(init));
    }

    case TokenType::sym_colon: {
      auto type = identifier_str();

      must_consume(
        TokenType::sym_semicolon,
        "expected `;` after variable declaration");

      return std::make_unique<Stmt::Variable>(name, type);
    }

    default: {
      throw_error(
        Severity::error, "expected `=` or `:` after variable declaration");

      // GARBAGE!! C++ doesn't look ONE FUNCTION in
      // to see that this code path will ALWAYS throw
      // BUT we MUST satisfy the compiler otherwise
      // it will give us a WARNING
      return std::make_unique<Stmt::Variable>(name, expr());
    }
  }
}

auto Parser::decl_function() -> Stmt::Stmt {
  if (!match(TokenType::identifier)) {
    backward();
    return stmt_exprstmt();
  }

  auto name       = identifier_str();
  auto parameters = std::vector<std::string> {};
  auto body       = empty_stmt();

  if (consume(TokenType::sym_lround)) { parameters = parameters_str(); }

  if (consume(TokenType::kw_virt)) {
    must_consume(TokenType::sym_semicolon, "expected `;` after virt fct");
    return std::make_unique<Stmt::Function>(
      name, parameters, std::move(body), true);
  }

  must_consume(TokenType::sym_lbrace, "missing `{`");

  body = stmt_block();

  return std::make_unique<Stmt::Function>(
    name, parameters, std::move(body), false);
}

auto Parser::decl_struct() -> Stmt::Stmt {
  auto name   = identifier_str();
  auto parent = std::string {};
  auto body   = std::vector<Stmt::Stmt> {};

  if (consume(TokenType::sym_slash)) { parent = identifier_str(); }

  must_consume(TokenType::sym_lbrace, "missing struct body");

  while (!consume(TokenType::sym_rbrace)) {
    body.emplace_back(decl());
  }

  return std::make_unique<Stmt::Struct>(name, parent, std::move(body));
}

auto Parser::decl_ctor() -> Stmt::Stmt {
  auto parameters = std::vector<std::string> {};

  if (consume(TokenType::sym_lround)) { parameters = parameters_str(); }

  must_consume(TokenType::sym_lbrace, "missing `{`");

  auto body = stmt_block();

  return std::make_unique<Stmt::Constructor>(parameters, std::move(body));
}

auto Parser::decl_dtor() -> Stmt::Stmt {
  must_consume(TokenType::sym_lbrace, "missing `{`");
  auto body = stmt_block();
  return std::make_unique<Stmt::Destructor>(std::move(body));
}

auto Parser::decl_export() -> Stmt::Stmt {
  auto exported = empty_stmt();

  switch (advance().type()) {
    case TokenType::kw_let: exported = decl_variable();
    case TokenType::kw_struct: exported = decl_struct();
    case TokenType::kw_function: exported = decl_function();
    default: {
      throw_error(Severity::error, "invalid export declaration");
      return empty_stmt();
    }
  }
  return std::make_unique<Stmt::Export>(std::move(exported));
}

auto Parser::expr() -> Expr::Expr {
  return expr_assignment();
}

auto Parser::stmt() -> Stmt::Stmt {
  switch (advance().type()) {
    case TokenType::kw_entrypoint: return stmt_entrypoint();
    case TokenType::kw_package: return stmt_package();
    case TokenType::kw_import: return stmt_import();
    case TokenType::kw_forwarding: return stmt_forwarding();
    case TokenType::kw_if: return stmt_if();
    case TokenType::kw_for: return stmt_for();
    case TokenType::kw_break: return stmt_break();
    case TokenType::kw_continue: return stmt_continue();
    case TokenType::kw_return: return stmt_return();
    case TokenType::sym_lbrace: return stmt_block();

    default: {
      backward();
      return stmt_exprstmt();
    }
  }
}

auto Parser::decl() -> Stmt::Stmt {
  switch (advance().type()) {
    case TokenType::kw_let: return decl_variable();
    case TokenType::kw_function: return decl_function();
    case TokenType::kw_struct: return decl_struct();
    case TokenType::kw_ctor: return decl_ctor();
    case TokenType::kw_dtor: return decl_dtor();
    case TokenType::kw_export: return decl_export();

    default: {
      backward();
      return stmt();
    }
  }
}

auto Parser::program() -> std::vector<Stmt::Stmt> {
  std::vector<Stmt::Stmt> stmts {};

  while (!eof()) {
    try {
      stmts.emplace_back(decl());
    } catch (ParsingError e) {
      _error_callback(e);

      while (!eof() && !consume(TokenType::sym_semicolon)) {
        advance();
      }
    }
  }

  return std::move(stmts);
}

auto Parser::parse(Iter begin, Iter end) noexcept -> AST {
  this->tok = begin;
  this->end = end;
  return AST {std::move(program())};
}
