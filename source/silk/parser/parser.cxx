#include <optional>
#include <silk/parser/parser.h>

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <vector>

#include <silk/parser/ast.h>
#include <silk/parser/token.h>
#include <silk/util/error.h>

const std::unordered_map<TokenKind, Parser::Rule> Parser::rules = {
  // GROUPING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=

  {
    TokenKind::SYM_LROUND,
    {
      .prefix = &Parser::expr_grouping,
      .infix  = &Parser::expr_call,
      .prec   = Precedence::CALL,
    },
  },

  {
    TokenKind::SYM_LSQUARE,
    {
      .prefix = &Parser::expr_array,
    },
  },

  {
    TokenKind::SYM_LT,
    {
      .prefix = &Parser::expr_vector,
    },
  },

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-

  // LOGICAL OPERATIONS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  {
    TokenKind::SYM_BANG,
    {
      .prefix = &Parser::expr_unary,
    },
  },

  {
    TokenKind::SYM_EQUALEQUAL,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::EQUALITY,
    },
  },

  {
    TokenKind::SYM_BANGEQUAL,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::EQUALITY,
    },
  },

  {
    TokenKind::SYM_GT,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::COMPARISON,
    },
  },
  {
    TokenKind::SYM_GTEQUAL,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::COMPARISON,
    },
  },

  {
    TokenKind::SYM_LT,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::COMPARISON,
    },
  },
  {
    TokenKind::SYM_LTEQUAL,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::COMPARISON,
    },
  },

  {
    TokenKind::SYM_AMPAMP,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::AND,
    },
  },

  {
    TokenKind::SYM_PIPEPIPE,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::OR,
    },
  },

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-

  // ARITHMETIC OPERATIONS -=-=-=-=-=-=-=-=-=-=-=-=-=-=

  {
    TokenKind::SYM_MINUS,
    {
      .prefix = &Parser::expr_unary,
      .infix  = &Parser::expr_binary,
      .prec   = Precedence::TERM,
    },
  },
  {
    TokenKind::SYM_PLUS,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::TERM,
    },
  },
  {
    TokenKind::SYM_STAR,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::FACTOR,
    },
  },
  {
    TokenKind::SYM_SLASH,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::FACTOR,
    },
  },
  {
    TokenKind::SYM_STARSTAR,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::POWER,
    },
  },
  {
    TokenKind::SYM_SLASHSLASH,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::FACTOR,
    },
  },
  {
    TokenKind::SYM_PERCENT,
    {
      .infix = &Parser::expr_binary,
      .prec  = Precedence::FACTOR,
    },
  },

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-

  // LITERALS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  {
    TokenKind::IDENTIFIER,
    {
      .prefix = &Parser::expr_identifier,
    },
  },

  {
    TokenKind::LITERAL_INTEGER,
    {
      .prefix = &Parser::expr_literal,
    },
  },

  {
    TokenKind::LITERAL_DOUBLE,
    {
      .prefix = &Parser::expr_literal,
    },
  },

  {
    TokenKind::LITERAL_CHAR,
    {
      .prefix = &Parser::expr_literal,
    },
  },

  {
    TokenKind::LITERAL_STRING,
    {
      .prefix = &Parser::expr_literal,
    },
  },

  {
    TokenKind::BOOL_TRUE,
    {
      .prefix = &Parser::expr_literal,
    },
  },

  {
    TokenKind::BOOL_FALSE,
    {
      .prefix = &Parser::expr_literal,
    },
  },

  {
    TokenKind::KEY_VOID,
    {
      .prefix = &Parser::expr_literal,
    },
  },

  {
    TokenKind::KEY_PI,
    {
      .prefix = &Parser::expr_literal,
    },
  },

  {
    TokenKind::KEY_TAU,
    {
      .prefix = &Parser::expr_literal,
    },
  },

  {
    TokenKind::KEY_EUL,
    {
      .prefix = &Parser::expr_literal,
    },
  },

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-

  {
    TokenKind::SYM_EQUAL,
    {
      .infix = &Parser::expr_assignment,
      .prec  = Precedence::ASSIGNMENT,
    },
  },

  {
    TokenKind::KW_FUN,
    {
      .prefix = &Parser::expr_lambda,
    },
  },
};

inline auto Parser::eof() const -> bool {
  return _next.kind == TokenKind::TOK_END;
}

inline auto Parser::advance() -> Token & {
  _prev = _next;
  _next = _scanner.scan();

  return _prev;
}

inline auto Parser::must_match(TokenKind kind, std::string msg) const -> void {
  if (!match(kind)) { throw report(_prev.location, msg); }
}

inline auto Parser::must_consume(TokenKind kind, std::string msg) -> void {
  if (!consume(kind)) { throw report(_prev.location, msg); }
}

auto Parser::get_rule(const Token &tok) const
  -> std::optional<std::reference_wrapper<const Rule>> {
  if (rules.find(tok.kind) == rules.end()) return {};
  return rules.at(tok.kind);
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
  auto rule = get_rule(_next);

  if (!rule) throw report(_next.location, "rule not found");

  auto prefix_fn = rule->get().prefix;
  auto prefix    = (this->*prefix_fn)();

  for (auto rule = get_rule(_next);
       rule && rule->get().infix && prec <= rule->get().prec;
       rule = get_rule(_next)) {
    prefix = std::move((this->*(rule->get().infix))(std::move(prefix)));
  }

  return std::move(prefix);
}

auto Parser::next_assign() -> bool {
  return match(
    TokenKind::SYM_EQUAL, TokenKind::SYM_EQUALEQUAL, TokenKind::SYM_MINUSEQUAL);
}

auto Parser::parse_name() -> std::string {
  must_match(TokenKind::IDENTIFIER, "expected a name");
  return advance().lexeme;
}

auto Parser::parse_package() -> std::string {
  must_match(TokenKind::LITERAL_STRING, "expected a package name");
  return advance().lexeme;
}

auto Parser::parse_typing() -> Typing {
  // TODO
  if (!consume(TokenKind::SYM_COLONCOLON)) {
    // no typing
    return nullptr;
  }

  // typing!
  return nullptr;
}

auto Parser::parse_typed_fields(TokenKind end, TokenKind delim) -> TypedFields {
  auto fields = TypedFields{};

  while (!consume(end)) {
    fields.emplace_back(parse_name(), parse_typing());
    if (!match(end)) must_consume(delim, "expected a delimiter");
  }

  return fields;
}

auto Parser::declaration() -> Statement {
  switch (_next.kind) {
    case TokenKind::KW_PKG: [[fallthrough]];
    case TokenKind::KW_USE: return decl_package();
    case TokenKind::KW_CONST: return decl_constant();
    case TokenKind::KW_FUN: return decl_function();
    case TokenKind::KW_ENUM: return decl_enum();
    case TokenKind::KW_STRUCT: return decl_struct();
    case TokenKind::KW_MAIN: return decl_main();
    default: throw report(_prev.location, "expected declaration");
  }
}

auto Parser::decl_package() -> Statement {
  auto action = [&] {
    switch (advance().kind) {
      case TokenKind::KW_PKG: return Package::DECLARE;
      case TokenKind::KW_USE: return Package::IMPORT;
      default: throw report(_prev.location, "expecting `pkg` or `use`");
    }
  }();

  auto package = parse_package();

  return std::make_unique<Package>(std::move(package), action);
}

auto Parser::decl_constant() -> Statement {
  must_consume(TokenKind::KW_CONST, "expected `const`");

  auto name   = parse_name();
  auto typing = parse_typing();

  must_consume(TokenKind::SYM_EQUAL, "expected `=` after variable declaration");

  auto init = expression();

  return std::make_unique<Constant>(std::move(name), typing, std::move(init));
}

auto Parser::decl_function() -> Statement {
  must_consume(TokenKind::KW_FUN, "expected `fun`");

  auto name = parse_name();

  auto params =
    consume(TokenKind::SYM_LROUND)
      ? parse_typed_fields(TokenKind::SYM_RROUND, TokenKind::SYM_COMMA)
      : TypedFields{};

  auto return_type = parse_typing();

  auto body = consume(TokenKind::SYM_EQUAL)
                ? std::make_unique<Return>(expression())
                : stmt_block();

  return std::make_unique<Function>(
    std::move(name),
    make_node<Lambda>(return_type, std::move(params), std::move(body)));
}

auto Parser::decl_enum() -> Statement {
  must_consume(TokenKind::KW_ENUM, "expected `enum`");

  auto name = parse_name();

  must_consume(TokenKind::SYM_LBRACE, "expected `{`");

  auto variants =
    parse_typed_fields(TokenKind::SYM_RBRACE, TokenKind::SYM_SEMICOLON);

  return std::make_unique<Enum>(std::move(name), std::move(variants));
}

auto Parser::decl_struct() -> Statement {
  must_consume(TokenKind::KW_STRUCT, "expected `struct`");

  auto name = parse_name();

  must_consume(TokenKind::SYM_LBRACE, "expected `{`");

  auto fields = TypedFields{};

  while (!match(TokenKind::SYM_RBRACE, TokenKind::KW_FUN)) {
    auto name   = parse_name();
    auto typing = parse_typing();
    fields.push_back({name, typing});

    if (!match(TokenKind::SYM_RBRACE, TokenKind::KW_FUN)) {
      must_consume(TokenKind::SYM_SEMICOLON, "expected `;`");
    }
  }

  auto methods = std::vector<Statement>{};

  while (!consume(TokenKind::SYM_RBRACE)) {
    methods.push_back(decl_function());
  }

  return std::make_unique<Struct>(
    std::move(name), std::move(fields), std::move(methods));
}

auto Parser::decl_main() -> Statement {
  must_consume(TokenKind::KW_MAIN, "expected `main`");

  auto body = std::vector<Statement>{};

  must_consume(TokenKind::SYM_LBRACE, "expected `{`");
  while (!consume(TokenKind::SYM_RBRACE)) {
    body.push_back(statement());
  }

  return std::make_unique<Main>(std::move(body));
}

auto Parser::statement() -> Statement {
  switch (_next.kind) {
    case TokenKind::KW_DEF: [[fallthrough]];
    case TokenKind::KW_LET: return stmt_variable();

    case TokenKind::KW_IF: return stmt_conditional();
    case TokenKind::KW_FOR: return stmt_loop();
    case TokenKind::KW_MATCH: return stmt_match();

    case TokenKind::SYM_SEMICOLON: return stmt_empty();
    case TokenKind::SYM_LBRACE: return stmt_block();

    case TokenKind::KW_CONTINUE: [[fallthrough]];
    case TokenKind::KW_BREAK: return stmt_control_flow();
    case TokenKind::KW_RETURN: return stmt_return();

    default: return stmt_exprstmt();
  }
}

auto Parser::stmt_empty() -> Statement {
  must_consume(TokenKind::SYM_SEMICOLON, "expected `;`");
  return std::make_unique<Empty>();
}

auto Parser::stmt_variable() -> Statement {
  auto immutable = [&] {
    switch (advance().kind) {
      case TokenKind::KW_LET: return true;
      case TokenKind::KW_DEF: return false;
      default: throw report(_prev.location, "expected let or def");
    }
  }();

  auto name   = parse_name();
  auto typing = parse_typing();

  must_consume(TokenKind::SYM_EQUAL, "expected `=` after variable declaration");

  auto init = expression();

  return std::make_unique<Variable>(
    std::move(name), typing, std::move(init), immutable);
}

auto Parser::stmt_exprstmt() -> Statement {
  auto stmt = std::make_unique<ExprStmt>(expression());
  must_consume(TokenKind::SYM_SEMICOLON, "expected `;`");
  return stmt;
}

auto Parser::stmt_block() -> Statement {
  auto body = std::vector<Statement>{};

  must_consume(TokenKind::SYM_LBRACE, "expected `{`");
  while (!consume(TokenKind::SYM_RBRACE)) {
    body.push_back(statement());
  }

  return std::make_unique<Block>(std::move(body));
}

auto Parser::stmt_conditional() -> Statement {
  must_consume(TokenKind::KW_IF, "expected if");

  auto clause = expression();
  auto conseq = stmt_block();
  auto altern = consume(TokenKind::KW_ELSE) //
                  ? stmt_block()
                  : std::make_unique<Empty>();

  return std::make_unique<Conditional>(
    std::move(clause), std::move(conseq), std::move(altern));
}

auto Parser::stmt_loop() -> Statement {
  must_consume(TokenKind::KW_FOR, "expected `for`");

  auto clause = match(TokenKind::SYM_LBRACE) //
                  ? make_node<BoolLiteral>(true)
                  : expression();

  auto body = stmt_block();

  return std::make_unique<Loop>(std::move(clause), std::move(body));
}

auto Parser::stmt_match() -> Statement {
  must_match(TokenKind::KW_MATCH, "expected match");

  auto target = expression();
  auto cases  = std::vector<Statement>{};

  must_consume(TokenKind::SYM_LBRACE, "expected {");

  while (!consume(TokenKind::SYM_RBRACE)) {
    cases.push_back(stmt_match_case());
  }

  return std::make_unique<Match>(std::move(target), std::move(cases));
}

auto Parser::stmt_match_case() -> Statement {
  auto expr = expression();

  must_match(TokenKind::SYM_ARROW, "expected `->`");

  auto body = stmt_block();

  return std::make_unique<MatchCase>(std::move(expr), std::move(body));
}

auto Parser::stmt_control_flow() -> Statement {
  switch (advance().kind) {
    case TokenKind::KW_BREAK:
      return std::make_unique<ControlFlow>(ControlFlow::BREAK);
    case TokenKind::KW_CONTINUE:
      return std::make_unique<ControlFlow>(ControlFlow::CONTINUE);
    default: throw report(_prev.location, "expected `break` or `continue`");
  }
}

auto Parser::stmt_return() -> Statement {
  must_consume(TokenKind::KW_RETURN, "expected `return`");

  auto return_value = [&] {
    if (match(TokenKind::SYM_SEMICOLON)) {
      return make_node<KeyLiteral>(KeyLiteral::VOID);
    }

    return expression();
  }();

  must_consume(TokenKind::SYM_SEMICOLON, "expected `;`");
  return std::make_unique<Return>(std::move(return_value));
}

auto Parser::expression() -> ASTNode {
  if (eof()) throw report(_prev.location, "expected expression");
  return precendece(Precedence::ASSIGNMENT);
}

auto Parser::expr_identifier() -> ASTNode {
  must_consume(TokenKind::IDENTIFIER, "expected identifier");
  const auto name = _prev.lexeme;
  const auto type = next_assign() ? Identifier::REF : Identifier::VAL;
  return make_node<Identifier>(type, name);
}

auto Parser::expr_unary() -> ASTNode {
  auto tok     = advance();
  auto operand = precendece(Precedence::UNARY);
  return make_node<Unary>(tok.kind, std::move(operand));
}

auto Parser::expr_binary(ASTNode &&left) -> ASTNode {
  auto tok   = advance();
  auto rule  = get_rule(tok);
  auto right = precendece(higher(rule->get().prec));

  return make_node<Binary>(tok.kind, std::move(left), std::move(right));
}

auto Parser::expr_literal() -> ASTNode {
  switch (advance().kind) {
    case TokenKind::KEY_VOID: return make_node<KeyLiteral>(KeyLiteral::VOID);
    case TokenKind::KEY_PI: return make_node<KeyLiteral>(KeyLiteral::PI);
    case TokenKind::KEY_TAU: return make_node<KeyLiteral>(KeyLiteral::TAU);
    case TokenKind::KEY_EUL: return make_node<KeyLiteral>(KeyLiteral::EULER);
    case TokenKind::BOOL_TRUE: return make_node<BoolLiteral>(true);
    case TokenKind::BOOL_FALSE: return make_node<BoolLiteral>(false);
    case TokenKind::LITERAL_INTEGER:
      return make_node<IntLiteral>(std::atoi(_prev.lexeme.c_str()));
    case TokenKind::LITERAL_DOUBLE:
      return make_node<RealLiteral>(std::atof(_prev.lexeme.c_str()));
    case TokenKind::LITERAL_CHAR:
      return make_node<CharLiteral>(_prev.lexeme.at(0));
    case TokenKind::LITERAL_STRING:
      return make_node<StringLiteral>(std::move(_prev.lexeme));
    default: throw report(_prev.location, "expected constant or literal");
  }
}

auto Parser::expr_lambda() -> ASTNode {
  must_consume(TokenKind::KW_FUN, "expected `fun`");

  // TODO
  return make_node<KeyLiteral>(KeyLiteral::VOID);
}

auto Parser::expr_assignment(ASTNode &&target) -> ASTNode {
  const auto type = [&] {
    switch (advance().kind) {
      case TokenKind::SYM_EQUAL: return Assignment::ASSIGN;
      case TokenKind::SYM_PLUSEQUAL: return Assignment::ADD;
      case TokenKind::SYM_MINUSEQUAL: return Assignment::SUBTRACT;
      default: throw report(_prev.location, "expected `=`, `+=` or `-=`");
    }
  }();

  return make_node<Assignment>(type, std::move(target), expression());
}

auto Parser::expr_call(ASTNode &&left) -> ASTNode {
  must_consume(TokenKind::SYM_LROUND, "expected `(`");

  auto args = std::vector<ASTNode>{};

  while (!consume(TokenKind::SYM_RROUND)) {
    args.push_back(expression());

    if (!match(TokenKind::SYM_RROUND)) {
      must_consume(TokenKind::SYM_COMMA, "expected `,`");
    }
  }

  return make_node<Call>(std::move(left), std::move(args));
}

auto Parser::expr_grouping() -> ASTNode {
  must_consume(TokenKind::SYM_LROUND, "expected opening parenthesis `)`");
  auto inner = expression();
  must_consume(TokenKind::SYM_RROUND, "expected closing parenthesis `)`");
  return std::move(inner);
}

auto Parser::expr_array() -> ASTNode {
  auto contents = std::vector<ASTNode>{};

  must_consume(TokenKind::SYM_LSQUARE, "expected `]`");

  while (!consume(TokenKind::SYM_RSQUARE)) {
    contents.push_back(expression());

    if (!match(TokenKind::SYM_RSQUARE)) {
      must_match(TokenKind::SYM_COMMA, "expected `,`");
    }
  }

  return make_node<ArrayLiteral>(std::move(contents));
}

auto Parser::expr_vector() -> ASTNode {
  auto contents = std::vector<ASTNode>{};

  must_consume(TokenKind::SYM_LT, "expected `<`");

  while (!consume(TokenKind::SYM_GT)) {
    contents.push_back(expression());

    if (!match(TokenKind::SYM_GT)) {
      must_consume(TokenKind::SYM_COMMA, "expected `,`");
    }
  }

  return make_node<VectorLiteral>(std::move(contents));
}

// public compile function

auto Parser::parse_source() noexcept -> AST {
  // Create an AST to push declarations onto
  auto ast = AST{};

  while (!eof()) {
    try {
      ast.push_back(std::move(declaration()));
    } catch (...) {
      while (!eof() && !consume(TokenKind::SYM_SEMICOLON)) {
        advance();
      }
    }
  }

  return ast;
}

auto Parser::parse_line() noexcept -> AST {
  // Create an AST to push declarations onto
  auto ast = AST{};

  try {
    ast.push_back(std::move(declaration()));
  } catch (...) {
    clear_errors();

    try {
      ast.push_back(std::make_unique<ExprStmt>(expression()));
    } catch (...) {}
  }

  return ast;
}
