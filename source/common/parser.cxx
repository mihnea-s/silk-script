#include <cstdint>
#include <cstdlib>
#include <memory>
#include <vector>

#include <silk/common/ast.h>
#include <silk/common/error.h>
#include <silk/common/errors.h>
#include <silk/common/parser.h>
#include <silk/common/token.h>

const Parser::Rule Parser::no_rule = {};

const std::unordered_map<TokenType, Parser::Rule> Parser::rules = {
  // GROUPING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=

  {
    TokenType::sym_lround,
    {
      .prefix = &Parser::expr_grouping,
      .infix  = &Parser::expr_call,
      .prec   = Precedence::CALL,
    },
  },

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-

  // LOGICAL OPERATIONS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  {
    TokenType::sym_bang,
    {
      .prefix = &Parser::expr_unary,
    },
  },

  {
    TokenType::sym_equalequal,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::EQUALITY,
    },
  },

  {
    TokenType::sym_bangequal,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::EQUALITY,
    },
  },

  {
    TokenType::sym_gt,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::COMPARISON,
    },
  },
  {
    TokenType::sym_gtequal,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::COMPARISON,
    },
  },

  {
    TokenType::sym_lt,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::COMPARISON,
    },
  },
  {
    TokenType::sym_ltequal,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::COMPARISON,
    },
  },

  {
    TokenType::sym_ampamp,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::AND,
    },
  },

  {
    TokenType::sym_pipepipe,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::OR,
    },
  },

  {
    TokenType::kw_is,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::OR,
    },
  },

  {
    TokenType::kw_isnt,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::OR,
    },
  },

  {
    TokenType::kw_typeof,
    {
      .prefix = &Parser::expr_unary,
      .prec   = Precedence::UNARY,
    },
  },

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-

  // ARITHMETIC OPERATIONS -=-=-=-=-=-=-=-=-=-=-=-=-=-=

  {
    TokenType::sym_minus,
    {
      .prefix = &Parser::expr_unary,
      .infix  = &Parser::expr_binary,
      .prec   = Precedence::TERM,
    },
  },
  {
    TokenType::sym_plus,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::TERM,
    },
  },
  {
    TokenType::sym_star,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::FACTOR,
    },
  },
  {
    TokenType::sym_slash,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::FACTOR,
    },
  },
  {
    TokenType::sym_starstar,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::POWER,
    },
  },
  {
    TokenType::sym_slashslash,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::FACTOR,
    },
  },
  {
    TokenType::sym_percent,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::FACTOR,
    },
  },

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-

  // LITERALS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  {
    TokenType::identifier,
    {
      .prefix = &Parser::expr_identifier,
    },
  },

  {
    TokenType::literal_int,
    {
      .prefix = &Parser::literal_integer,
    },
  },

  {
    TokenType::literal_dbl,
    {
      .prefix = &Parser::literal_real,
    },
  },

  {
    TokenType::literal_str,
    {
      .prefix = &Parser::literal_string,
    },
  },

  {
    TokenType::cnst_true,
    {
      .prefix = &Parser::constant_value,
    },
  },

  {
    TokenType::cnst_false,
    {
      .prefix = &Parser::constant_value,
    },
  },

  {
    TokenType::cnst_vid,
    {
      .prefix = &Parser::constant_value,
    },
  },

  {
    TokenType::cnst_pi,
    {
      .prefix = &Parser::constant_value,
    },
  },

  {
    TokenType::cnst_tau,
    {
      .prefix = &Parser::constant_value,
    },
  },

  {
    TokenType::cnst_eul,
    {
      .prefix = &Parser::constant_value,
    },
  },

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-

  {
    TokenType::sym_equal,
    {
      .infix = &Parser::expr_assignment,
      .prec  = Precedence::ASSIGNMENT,
    },
  },

  {
    TokenType::kw_const,
    {
      .prefix = &Parser::expr_const,
    },
  },
};

inline auto Parser::forward() -> void {
  _tok++;
}

inline auto Parser::backward() -> void {
  _tok--;
}

inline auto Parser::advance() -> const Token& {
  _tok++;
  return previous();
}

inline auto Parser::previous() const -> const Token& {
  return *(_tok - 1);
}

inline auto Parser::current() const -> const Token& {
  return *_tok;
}

inline auto Parser::eof() const -> bool {
  return _tok == _end;
}

inline auto Parser::error_location() const
  -> std::pair<std::uint64_t, std::uint64_t> {
  if (eof()) return previous().location();
  return current().location();
}

inline void Parser::should_match(TokenType type, std::string msg) const {
  if (!match(type)) { report_warning(error_location(), msg); }
}

inline void Parser::must_match(TokenType type, std::string msg) const {
  if (!match(type)) { throw report_error(error_location(), msg); }
}

inline void Parser::should_consume(TokenType type, std::string msg) {
  if (!consume(type)) { report_warning(error_location(), msg); }
}

inline void Parser::must_consume(TokenType type, std::string msg) {
  if (!consume(type)) { throw report_error(error_location(), msg); }
}

auto Parser::get_rule(const Token& tok) const -> const Rule& {
  if (rules.find(tok.type()) == rules.end()) {
    // report_warning(error_location(), "rule not found for: {}", tok.lexeme());
    return no_rule;
  }
  return rules.at(tok.type());
}

auto Parser::higher(Precedence prec) const -> Precedence {
  if (prec == Precedence::NONE) return prec;
  return static_cast<Precedence>(static_cast<int>(prec) + 1);
}

auto Parser::lower(Precedence prec) const -> Precedence {
  if (prec == Precedence::ANY) return prec;
  return static_cast<Precedence>(static_cast<int>(prec) - 1);
}

auto Parser::precendece(Precedence prec) -> ASTNodePtr {
  auto rule = get_rule(current());

  if (!rule.prefix) {
    throw report_error(
      error_location(), "rule not found for: {}", current().lexeme());
  }

  auto prefix = (this->*rule.prefix)();

  for (auto rule = get_rule(current()); !eof() && prec <= rule.prec;
       rule      = get_rule(current())) {
    prefix = (this->*rule.infix)(std::move(prefix));
  }

  return prefix;
}

auto Parser::next_assign() -> bool {
  return match(
    TokenType::sym_equal, TokenType::sym_plusequal, TokenType::sym_minusequal);
}

auto Parser::parse_name() -> std::string {
  must_match(
    TokenType::identifier, SilkErrors::expectedIdentif(current().lexeme()));
  return advance().lexeme();
}

auto Parser::parse_type() -> ASTType {
  must_match(
    TokenType::identifier, SilkErrors::expectedType(current().lexeme()));
  return advance().lexeme();
}

auto Parser::parse_pkg() -> std::string {
  must_match(
    TokenType::literal_str, SilkErrors::expectedStr(current().lexeme()));
  return advance().lexeme();
}

auto Parser::parse_parameters() -> ASTParameters {
  ASTParameters params {};

  while (!consume(TokenType::sym_rround)) {
    params.push_back(parse_name());

    if (!match(TokenType::sym_rround)) {
      must_consume(TokenType::sym_comma, SilkErrors::missingComma());
    }
  }

  return params;
}

auto Parser::declaration() -> ASTNodePtr {
  switch (current().type()) {
    case TokenType::kw_def: [[fallthrough]];
    case TokenType::kw_let: return decl_variable();

    case TokenType::kw_fct: return decl_function();
    case TokenType::kw_struct: return decl_struct();

    case TokenType::kw_pkg: [[fallthrough]];
    case TokenType::kw_use: [[fallthrough]];
    case TokenType::kw_main: return decl_package();

    default: return statement();
  }
}

auto Parser::statement() -> ASTNodePtr {
  switch (current().type()) {
    case TokenType::sym_semicolon: return stmt_empty();
    case TokenType::sym_lbrace: return stmt_block();
    case TokenType::kw_if: return stmt_conditional();
    case TokenType::kw_for: return stmt_loop();
    case TokenType::kw_match: return stmt_match();

    case TokenType::kw_continue: [[fallthrough]];
    case TokenType::kw_break: return stmt_interrupt();
    case TokenType::kw_return: return stmt_return();

    default: return stmt_exprstmt();
  }
}

auto Parser::expression() -> ASTNodePtr {
  if (eof()) throw report_error(error_location(), "expected expression");
  return precendece(Precedence::ASSIGNMENT);
}

auto Parser::decl_variable() -> ASTNodePtr {
  auto constant = false;

  switch (advance().type()) {
    case TokenType::kw_let: constant = true; break;
    case TokenType::kw_def: constant = false; break;
    default: throw report_error(previous().location(), "expected let or def");
  }

  auto name = parse_name();
  auto type = ASTType {};
  auto init = make_node<Vid>();

  // optional typing
  if (consume(TokenType::sym_colon)) { type = parse_type(); }

  // optional initializer expression
  if (consume(TokenType::sym_equal)) { init = expression(); }

  must_consume(TokenType::sym_semicolon, "expected `;` after declaration");

  return make_node<Variable>(name, type, std::move(init), constant);
}

auto Parser::decl_function() -> ASTNodePtr {
  must_consume(TokenType::kw_fct, "expected `fct`");

  auto name        = parse_name();
  auto return_type = ASTType {};
  auto parameters  = ASTParameters {};
  auto body        = (ASTNodePtr) nullptr;

  if (consume(TokenType::sym_lround)) parameters = parse_parameters();
  if (consume(TokenType::sym_colon)) return_type = parse_type();

  auto is_virtual = consume(TokenType::kw_virt);
  auto is_async   = consume(TokenType::kw_async);

  if (match(TokenType::sym_arrow)) {
    body = make_node<Return>(expression());
  } else if (match(TokenType::sym_lbrace)) {
    body = stmt_block();
  }

  return make_node<Function>(
    name,
    return_type,
    std::move(parameters),
    std::move(body),
    is_virtual,
    is_async);
}

auto Parser::decl_package() -> ASTNodePtr {
  auto tok = advance();
  switch (tok.type()) {
    case TokenType::kw_main: {
      must_consume(TokenType::sym_semicolon, SilkErrors::stmtSemicolon());
      return make_node<Package>(tok.lexeme(), Package::DECLARATION);
    }

    case TokenType::kw_pkg: {
      auto pkg_name = parse_pkg();
      must_consume(TokenType::sym_semicolon, SilkErrors::stmtSemicolon());
      return make_node<Package>(pkg_name, Package::DECLARATION);
    }

    case TokenType::kw_use: {
      auto import_name = parse_pkg();
      must_consume(TokenType::sym_semicolon, SilkErrors::stmtSemicolon());
      return make_node<Package>(import_name, Package::IMPORT);
    }

    default: throw report_error(error_location(), SilkErrors::invalidState());
  }
}

auto Parser::decl_struct() -> ASTNodePtr {
  return make_node<Vid>();
}

auto Parser::stmt_empty() -> ASTNodePtr {
  must_consume(TokenType::sym_semicolon, "expected `;`");
  return make_node<Empty>();
}

auto Parser::stmt_block() -> ASTNodePtr {
  auto body = ASTNodeList {};

  must_consume(TokenType::sym_lbrace, "expected `{`");
  while (!consume(TokenType::sym_rbrace)) {
    body.push_back(declaration());
  }

  return make_node<Block>(std::move(body));
}

auto Parser::stmt_conditional() -> ASTNodePtr {
  must_consume(TokenType::kw_if, "expected if");
  must_consume(TokenType::sym_lround, "expected (");
  auto clause = expression();
  must_consume(TokenType::sym_rround, "expected )");

  auto true_body  = statement();
  auto false_body = (ASTNodePtr) nullptr;

  if (consume(TokenType::kw_else)) { false_body = statement(); }

  return make_node<Conditional>(
    std::move(clause), std::move(true_body), std::move(false_body));
}

auto Parser::stmt_loop() -> ASTNodePtr {
  must_consume(TokenType::kw_for, "expected for");

  auto clause = make_node<BoolLiteral>(true);

  if (consume(TokenType::sym_lround)) {
    clause = expression();
    must_consume(TokenType::sym_rround, "expected )");
  }

  auto body = statement();

  return make_node<Loop>(std::move(clause), std::move(body));
}

auto Parser::stmt_match() -> ASTNodePtr {
  must_match(TokenType::kw_match, "expected match");
  must_consume(TokenType::sym_lround, "expected (");
  auto target = expression();
  must_consume(TokenType::sym_rround, "expected )");

  auto cases = ASTNodeList {};

  must_consume(TokenType::sym_lbrace, "expected {");
  while (!consume(TokenType::sym_rbrace)) {
    cases.push_back(stmt_matchcase());
  }

  return make_node<Match>(std::move(target), std::move(cases));
}

auto Parser::stmt_matchcase() -> ASTNodePtr {
  auto pattern = ASTNodePtr {nullptr};

  if (!consume(TokenType::sym_uscoreuscore)) pattern = expression();

  must_match(TokenType::sym_arrow, "expected arrow after case");

  auto body = statement();

  return make_node<MatchCase>(std::move(pattern), std::move(body));
}

auto Parser::stmt_interrupt() -> ASTNodePtr {
  switch (advance().type()) {
    case TokenType::kw_break: {
      return make_node<Interrupt>(Interrupt::BREAK);
    }

    case TokenType::kw_continue: {
      return make_node<Interrupt>(Interrupt::CONTINUE);
    }

    default: {
      throw report_error(error_location(), SilkErrors::expectedInterrupt());
    }
  }
}

auto Parser::stmt_return() -> ASTNodePtr {
  must_consume(TokenType::kw_return, "return expected");

  auto ret = make_node<Vid>();

  if (!match(TokenType::sym_semicolon)) ret = expression();

  must_consume(TokenType::sym_semicolon, SilkErrors::stmtSemicolon());
  return make_node<Return>(std::move(ret));
}

auto Parser::stmt_exprstmt() -> ASTNodePtr {
  auto stmt = make_node<ExprStmt>(expression());
  must_consume(TokenType::sym_semicolon, SilkErrors::stmtSemicolon());
  return std::move(stmt);
}

auto Parser::expr_unary() -> ASTNodePtr {
  auto tok     = advance();
  auto operand = precendece(Precedence::UNARY);
  return make_node<Unary>(tok.type(), std::move(operand));
}

auto Parser::expr_binary(ASTNodePtr left) -> ASTNodePtr {
  auto tok   = advance();
  auto rule  = get_rule(tok);
  auto right = precendece(higher(rule.prec));

  return make_node<Binary>(tok.type(), std::move(left), std::move(right));
}

auto Parser::expr_const() -> ASTNodePtr {
  must_consume(TokenType::kw_const, "expected `const`");
  must_consume(TokenType::sym_lsquare, "expected `[`");
  auto inner = expression();
  must_consume(TokenType::sym_rsquare, "expected `]`");
  return make_node<Grouping>(std::move(inner));
}

auto Parser::expr_call(ASTNodePtr left) -> ASTNodePtr {
  must_consume(TokenType::sym_lround, "expected opening parenthesis `(`");

  ASTNodeList args {};

  while (!consume(TokenType::sym_rround)) {
    args.push_back(expression());

    if (!match(TokenType::sym_rround)) {
      must_consume(TokenType::sym_comma, SilkErrors::missingComma());
    }
  }

  return make_node<Call>(std::move(left), std::move(args));
}

auto Parser::expr_grouping() -> ASTNodePtr {
  must_consume(TokenType::sym_lround, "expected opening parenthesis `)`");
  auto inner = expression();
  must_consume(TokenType::sym_rround, "expected closing parenthesis `)`");

  return make_node<Grouping>(std::move(inner));
}

auto Parser::expr_assignment(ASTNodePtr target) -> ASTNodePtr {
  switch (advance().type()) {
    case TokenType::sym_equal: {
      return make_node<Assignment>(
        std::move(target), expression(), Assignment::ASSIGN);
    }

    case TokenType::sym_plusequal: {
      return make_node<Assignment>(
        std::move(target), expression(), Assignment::ADD);
    }

    case TokenType::sym_minusequal: {
      return make_node<Assignment>(
        std::move(target), expression(), Assignment::SUBTRACT);
    }

    default: throw report_error(error_location(), SilkErrors::expectedAssign());
  }
}

auto Parser::expr_identifier() -> ASTNodePtr {
  must_consume(
    TokenType::identifier, SilkErrors::expectedIdentif(current().lexeme()));

  if (next_assign()) return make_node<IdentifierRef>(previous().lexeme());
  return make_node<IdentifierVal>(previous().lexeme());
}

auto Parser::literal_integer() -> ASTNodePtr {
  auto tok       = advance();
  auto int_value = std::atoi(tok.lexeme().c_str());
  return make_node<IntLiteral>(int_value);
}

auto Parser::literal_real() -> ASTNodePtr {
  auto tok        = advance();
  auto real_value = std::atof(tok.lexeme().c_str());
  return make_node<RealLiteral>(real_value);
}

auto Parser::literal_string() -> ASTNodePtr {
  auto tok = advance();
  auto str = tok.lexeme();
  return make_node<StringLiteral>(std::move(str));
}

auto Parser::constant_value() -> ASTNodePtr {
  auto tok = advance();
  switch (tok.type()) {
    case TokenType::cnst_true: return make_node<BoolLiteral>(true);
    case TokenType::cnst_false: return make_node<BoolLiteral>(false);
    case TokenType::cnst_vid: return make_node<Vid>();
    case TokenType::cnst_pi: return make_node<Constant>(Constant::PI);
    case TokenType::cnst_tau: return make_node<Constant>(Constant::TAU);
    case TokenType::cnst_eul: return make_node<Constant>(Constant::EULER);

    default: {
      throw report_error(error_location(), "expected constant or literal");
    }
  }
}

// public compile function

auto Parser::parse(Iter begin, Iter end) noexcept -> AST {
  this->_tok = begin;
  this->_end = end;

  ASTNodeList program;

  while (!eof()) {
    try {
      program.push_back(std::move(declaration()));
    } catch (...) {
      while (!eof() && !consume(TokenType::sym_semicolon)) {
        forward();
      }
    }
  }

  return AST {std::move(program)};
}
