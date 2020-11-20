#include <silk/parser/parser.h>

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <vector>

#include <silk/lexer/token.h>
#include <silk/parser/ast.h>
#include <silk/util/error.h>

const Parser::Rule Parser::no_rule = {};

const std::unordered_map<TokenType, Parser::Rule> Parser::rules = {
  // GROUPING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=

  {
    TokenType::SYM_LROUND,
    {
      .prefix = &Parser::expr_grouping,
      .infix  = &Parser::expr_call,
      .prec   = Precedence::CALL,
    },
  },

  {
    TokenType::SYM_LSQUARE,
    {
      .prefix = &Parser::expr_array,
    },
  },

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-

  // LOGICAL OPERATIONS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  {
    TokenType::SYM_BANG,
    {
      .prefix = &Parser::expr_unary,
    },
  },

  {
    TokenType::SYM_EQUALEQUAL,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::EQUALITY,
    },
  },

  {
    TokenType::SYM_BANGEQUAL,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::EQUALITY,
    },
  },

  {
    TokenType::SYM_GT,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::COMPARISON,
    },
  },
  {
    TokenType::SYM_GTEQUAL,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::COMPARISON,
    },
  },

  {
    TokenType::SYM_LT,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::COMPARISON,
    },
  },
  {
    TokenType::SYM_LTEQUAL,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::COMPARISON,
    },
  },

  {
    TokenType::SYM_AMPAMP,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::AND,
    },
  },

  {
    TokenType::SYM_PIPEPIPE,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::OR,
    },
  },

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-

  // ARITHMETIC OPERATIONS -=-=-=-=-=-=-=-=-=-=-=-=-=-=

  {
    TokenType::SYM_MINUS,
    {
      .prefix = &Parser::expr_unary,
      .infix  = &Parser::expr_binary,
      .prec   = Precedence::TERM,
    },
  },
  {
    TokenType::SYM_PLUS,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::TERM,
    },
  },
  {
    TokenType::SYM_STAR,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::FACTOR,
    },
  },
  {
    TokenType::SYM_SLASH,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::FACTOR,
    },
  },
  {
    TokenType::SYM_STARSTAR,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::POWER,
    },
  },
  {
    TokenType::SYM_SLASHSLASH,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::FACTOR,
    },
  },
  {
    TokenType::SYM_PERCENT,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::FACTOR,
    },
  },

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-

  // LITERALS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  {
    TokenType::IDENTIFIER,
    {
      .prefix = &Parser::expr_identifier,
    },
  },

  {
    TokenType::LITERAL_INTEGER,
    {
      .prefix = &Parser::expr_literal,
    },
  },

  {
    TokenType::LITERAL_DOUBLE,
    {
      .prefix = &Parser::expr_literal,
    },
  },

  {
    TokenType::LITERAL_CHAR,
    {
      .prefix = &Parser::expr_literal,
    },
  },

  {
    TokenType::LITERAL_STRING,
    {
      .prefix = &Parser::expr_literal,
    },
  },

  {
    TokenType::CNST_TRUE,
    {
      .prefix = &Parser::expr_const,
    },
  },

  {
    TokenType::CNST_FALSE,
    {
      .prefix = &Parser::expr_const,
    },
  },

  {
    TokenType::CNST_VOID,
    {
      .prefix = &Parser::expr_const,
    },
  },

  {
    TokenType::CNST_PI,
    {
      .prefix = &Parser::expr_const,
    },
  },

  {
    TokenType::CNST_TAU,
    {
      .prefix = &Parser::expr_const,
    },
  },

  {
    TokenType::CNST_EUL,
    {
      .prefix = &Parser::expr_const,
    },
  },

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-

  {
    TokenType::SYM_EQUAL,
    {
      .infix = &Parser::expr_assignment,
      .prec  = Precedence::ASSIGNMENT,
    },
  },

  {
    TokenType::KW_CONST,
    {
      .prefix = &Parser::expr_const,
    },
  },

  {
    TokenType::KW_FUN,
    {
      .prefix = &Parser::expr_lambda,
    },
  },
};

inline auto Parser::advance() -> const Token & {
  _tok++;
  return previous();
}

inline auto Parser::previous() const -> const Token & {
  return *(_tok - 1);
}

inline auto Parser::current() const -> const Token & {
  return *_tok;
}

inline auto Parser::eof() const -> bool {
  return _tok == _end;
}

inline auto Parser::error_location() const
  -> std::pair<std::uint64_t, std::uint64_t> {
  if (eof()) return previous().location;
  return current().location;
}

inline auto Parser::must_match(TokenType type, std::string msg) const -> void {
  if (!match(type)) { throw report(error_location(), msg); }
}

inline auto Parser::must_consume(TokenType type, std::string msg) -> void {
  if (!consume(type)) { throw report(error_location(), msg); }
}

auto Parser::get_rule(const Token &tok) const -> const Rule & {
  return rules.find(tok.type) == rules.end() ? no_rule : rules.at(tok.type);
}

auto Parser::higher(Precedence prec) const -> Precedence {
  if (prec == Precedence::NONE) return prec;
  return static_cast<Precedence>(static_cast<int>(prec) + 1);
}

auto Parser::lower(Precedence prec) const -> Precedence {
  if (prec == Precedence::ANY) return prec;
  return static_cast<Precedence>(static_cast<int>(prec) - 1);
}

auto Parser::precendece(Precedence prec) -> ASTNode {
  auto rule = get_rule(current());

  if (!rule.prefix) {
    throw report(error_location(), "rule not found for: {}", current().lexeme);
  }

  auto prefix = (this->*rule.prefix)();

  for (auto rule = get_rule(current()); !eof() && prec <= rule.prec;
       rule      = get_rule(current())) {
    prefix = std::move((this->*rule.infix)(std::move(prefix)));
  }

  return std::move(prefix);
}

auto Parser::next_assign() -> bool {
  return match(
    TokenType::SYM_EQUAL, TokenType::SYM_EQUALEQUAL, TokenType::SYM_MINUSEQUAL);
}

auto Parser::parse_name() -> std::string {
  must_match(TokenType::IDENTIFIER, "expected a name");
  return advance().lexeme;
}

auto Parser::parse_package() -> std::string {
  must_match(TokenType::LITERAL_STRING, "expected a package name");
  return advance().lexeme;
}

auto Parser::parse_typing() -> Typing {
  // TODO
  // must_match(TokenType::identifier, "expected a type");
  // return advance().lexeme();

  if (!consume(TokenType::SYM_COLONCOLON)) {
    // no typing
    return nullptr;
  }

  // typing!
  return nullptr;
}

auto Parser::parse_typed_fields(TokenType end, TokenType delim) -> TypedFields {
  auto fields = TypedFields{};

  while (!consume(end)) {
    fields.emplace_back(parse_name(), parse_typing());
    if (!match(end)) must_consume(delim, "expected a delimiter");
  }

  return fields;
}

auto Parser::declaration() -> Statement {
  switch (current().type) {

    case TokenType::KW_PKG: [[fallthrough]];
    case TokenType::KW_USE: return decl_package();

    case TokenType::KW_DEF: [[fallthrough]];
    case TokenType::KW_LET: return decl_variable();

    case TokenType::KW_MAIN: [[fallthrough]];
    case TokenType::KW_FUN: return decl_function();
    case TokenType::KW_STRUCT: return decl_struct();

    default: return statement();
  }
}

auto Parser::expression() -> ASTNode {
  if (eof()) throw report(error_location(), "expected expression");
  return precendece(Precedence::ASSIGNMENT);
}

auto Parser::decl_package() -> Statement {
  auto action = [&] {
    switch (advance().type) {
      case TokenType::KW_PKG: return Package::DECLARE;
      case TokenType::KW_USE: return Package::IMPORT;
      default: throw report(error_location(), "expecting `pkg` or `use`");
    }
  }();

  auto package = parse_package();

  return std::make_unique<Package>(std::move(package), action);
}

auto Parser::decl_variable() -> Statement {
  auto immutable = [&] {
    switch (advance().type) {
      case TokenType::KW_LET: return true;
      case TokenType::KW_DEF: return false;
      default: throw report(previous().location, "expected let or def");
    }
  }();

  auto name   = parse_name();
  auto typing = parse_typing();

  must_consume(TokenType::SYM_EQUAL, "expected `=` after variable declaration");

  auto init = expression();

  return std::make_unique<Variable>(
    std::move(name), typing, std::move(init), immutable);
}

auto Parser::decl_function() -> Statement {
  must_consume(TokenType::KW_FUN, "expected `fun`");

  auto name = parse_name();

  auto params = [&] {
    if (consume(TokenType::SYM_LROUND)) {
      return parse_typed_fields(TokenType::SYM_RROUND, TokenType::SYM_COMMA);
    }

    return TypedFields{};
  }();

  auto return_type = parse_typing();

  auto body = [&]() -> Statement {
    if (match(TokenType::SYM_EQUAL)) {
      return std::make_unique<Return>(expression());
    }

    must_match(TokenType::SYM_LBRACE, "expected `{`");
    return stmt_block();
  }();

  return std::make_unique<Function>(
    std::move(name),
    Lambda{
      return_type,
      std::move(params),
      std::move(body),
    });
}

auto Parser::decl_enum() -> Statement {
  must_consume(TokenType::KW_ENUM, "expected `enum`");

  auto name = parse_name();

  must_consume(TokenType::SYM_LBRACE, "expected `{`");

  auto variants =
    parse_typed_fields(TokenType::SYM_RBRACE, TokenType::SYM_SEMICOLON);

  return std::make_unique<Enum>(std::move(name), std::move(variants));
}

auto Parser::decl_struct() -> Statement {
  // TODO
  return std::make_unique<Empty>();
}

auto Parser::statement() -> Statement {
  switch (current().type) {
    case TokenType::SYM_SEMICOLON: return stmt_empty();
    case TokenType::SYM_LBRACE: return stmt_block();
    case TokenType::KW_IF: return stmt_conditional();
    case TokenType::KW_FOR: return stmt_loop();
    case TokenType::KW_MATCH: return stmt_match();

    case TokenType::KW_CONTINUE: [[fallthrough]];
    case TokenType::KW_BREAK: return stmt_control_flow();
    case TokenType::KW_RETURN: return stmt_return();

    default: return stmt_exprstmt();
  }
}

auto Parser::stmt_empty() -> Statement {
  must_consume(TokenType::SYM_SEMICOLON, "expected `;`");
  return std::make_unique<Empty>();
}

auto Parser::stmt_exprstmt() -> Statement {
  auto stmt = std::make_unique<ExprStmt>(expression());
  must_consume(TokenType::SYM_SEMICOLON, "expected `;`");
  return stmt;
}

auto Parser::stmt_block() -> Statement {
  auto body = std::vector<Statement>{};

  must_consume(TokenType::SYM_LBRACE, "expected `{`");
  while (!consume(TokenType::SYM_RBRACE)) {
    body.push_back(declaration());
  }

  return std::make_unique<Block>(std::move(body));
}

auto Parser::stmt_conditional() -> Statement {
  must_consume(TokenType::KW_IF, "expected if");
  must_consume(TokenType::SYM_LROUND, "expected (");
  auto clause = expression();
  must_consume(TokenType::SYM_RROUND, "expected )");

  auto conseq = statement();
  auto altern = [&]() -> Statement {
    if (consume(TokenType::KW_ELSE)) {
      return statement();
    } else {
      return std::make_unique<Empty>();
    }
  }();

  return std::make_unique<Conditional>(
    std::move(clause), std::move(conseq), std::move(altern));
}

auto Parser::stmt_loop() -> Statement {
  must_consume(TokenType::KW_FOR, "expected `for`");

  auto clause = [&]() -> ASTNode {
    if (consume(TokenType::SYM_LROUND)) {
      auto clause = expression();
      must_consume(TokenType::SYM_RROUND, "expected )");
      return std::move(clause);
    } else {
      return make_node<BoolLiteral>(true);
    }
  }();

  auto body = statement();

  return std::make_unique<Loop>(std::move(clause), std::move(body));
}

auto Parser::stmt_match() -> Statement {
  must_match(TokenType::KW_MATCH, "expected match");
  must_consume(TokenType::SYM_LROUND, "expected (");
  auto target = expression();
  must_consume(TokenType::SYM_RROUND, "expected )");

  auto cases = std::vector<Statement>{};

  must_consume(TokenType::SYM_LBRACE, "expected {");
  while (!consume(TokenType::SYM_RBRACE)) {
    cases.push_back(stmt_match_case());
  }

  return std::make_unique<Match>(std::move(target), std::move(cases));
}

auto Parser::stmt_match_case() -> Statement {
  // TODO
  return std::make_unique<Empty>();
}

auto Parser::stmt_control_flow() -> Statement {
  switch (advance().type) {
    case TokenType::KW_BREAK:
      return std::make_unique<ControlFlow>(ControlFlow::BREAK);
    case TokenType::KW_CONTINUE:
      return std::make_unique<ControlFlow>(ControlFlow::CONTINUE);
    default: throw report(error_location(), "expected `break` or `continue`");
  }
}

auto Parser::stmt_return() -> Statement {
  must_consume(TokenType::KW_RETURN, "expected `return`");

  auto return_value = [&] {
    if (match(TokenType::SYM_SEMICOLON)) {
      return make_node<Constant>(Constant::VOID);
    }

    return expression();
  }();

  must_consume(TokenType::SYM_SEMICOLON, "expected `;`");
  return std::make_unique<Return>(std::move(return_value));
}

auto Parser::expr_identifier() -> ASTNode {
  must_consume(TokenType::IDENTIFIER, "expected identifier");
  const auto name = previous().lexeme;
  const auto type = next_assign() ? Identifier::REF : Identifier::VAL;
  return make_node<Identifier>(type, name);
}

auto Parser::expr_unary() -> ASTNode {
  auto tok     = advance();
  auto operand = precendece(Precedence::UNARY);
  return make_node<Unary>(tok.type, std::move(operand));
}

auto Parser::expr_binary(ASTNode &&left) -> ASTNode {
  auto tok   = advance();
  auto rule  = get_rule(tok);
  auto right = precendece(higher(rule.prec));

  return make_node<Binary>(tok.type, std::move(left), std::move(right));
}

auto Parser::expr_const() -> ASTNode {
  switch (advance().type) {
    case TokenType::CNST_TRUE: return make_node<BoolLiteral>(true);
    case TokenType::CNST_FALSE: return make_node<BoolLiteral>(false);
    case TokenType::CNST_VOID: return make_node<Constant>(Constant::VOID);
    case TokenType::CNST_PI: return make_node<Constant>(Constant::PI);
    case TokenType::CNST_TAU: return make_node<Constant>(Constant::TAU);
    case TokenType::CNST_EUL: return make_node<Constant>(Constant::EULER);
    default: throw report(error_location(), "expected constant or literal");
  }
}

auto Parser::expr_literal() -> ASTNode {
  switch (advance().type) {
    case TokenType::LITERAL_INTEGER:
      return make_node<IntLiteral>(std::atoi(previous().lexeme.c_str()));
    case TokenType::LITERAL_DOUBLE:
      return make_node<RealLiteral>(std::atof(previous().lexeme.c_str()));
    case TokenType::LITERAL_CHAR:
      return make_node<CharLiteral>(previous().lexeme.at(0));
    case TokenType::LITERAL_STRING:
      return make_node<StringLiteral>(std::move(previous().lexeme));
    default: throw report(error_location(), "expected constant or literal");
  }
}

auto Parser::expr_lambda() -> ASTNode {
  must_consume(TokenType::KW_FUN, "expected `fun`");

  // TODO
  return make_node<Constant>(Constant::VOID);
}

auto Parser::expr_assignment(ASTNode &&target) -> ASTNode {
  const auto type = [&] {
    switch (advance().type) {
      case TokenType::SYM_EQUAL: return Assignment::ASSIGN;
      case TokenType::SYM_PLUSEQUAL: return Assignment::ADD;
      case TokenType::SYM_MINUSEQUAL: return Assignment::SUBTRACT;
      default: throw report(error_location(), "expected `=`, `+=` or `-=`");
    }
  }();

  return make_node<Assignment>(type, std::move(target), expression());
}

auto Parser::expr_call(ASTNode &&left) -> ASTNode {
  must_consume(TokenType::SYM_LROUND, "expected `(`");

  auto args = std::vector<ASTNode>{};

  while (!consume(TokenType::SYM_RROUND)) {
    args.push_back(expression());

    if (!match(TokenType::SYM_RROUND)) {
      must_consume(TokenType::SYM_COMMA, "expected `,`");
    }
  }

  return make_node<Call>(std::move(left), std::move(args));
}

auto Parser::expr_grouping() -> ASTNode {
  must_consume(TokenType::SYM_LROUND, "expected opening parenthesis `)`");
  auto inner = expression();
  must_consume(TokenType::SYM_RROUND, "expected closing parenthesis `)`");
  return std::move(inner);
}

auto Parser::expr_array() -> ASTNode {
  auto contents = std::vector<ASTNode>{};

  must_consume(TokenType::SYM_LSQUARE, "expected `]`");

  while (!consume(TokenType::SYM_RSQUARE)) {
    contents.push_back(expression());
  }

  return make_node<ArrayLiteral>(std::move(contents));
}

auto Parser::expr_constexpr() -> ASTNode {
  must_consume(TokenType::KW_CONST, "expected `const`");
  must_consume(TokenType::SYM_LBRACE, "expected `{`");
  auto inner = expression();
  must_consume(TokenType::SYM_RBRACE, "expected `}`");
  return make_node<ConstExpr>(std::move(inner));
}

// public compile function

auto Parser::parse(Iter begin, Iter end) noexcept -> AST {
  this->_tok = begin;
  this->_end = end;

  auto ast = AST{};

  while (!eof()) {
    try {
      ast.push_back(std::move(declaration()));
    } catch (...) {
      while (!eof() && !consume(TokenType::SYM_SEMICOLON)) {
        advance();
      }
    }
  }

  return ast;
}
