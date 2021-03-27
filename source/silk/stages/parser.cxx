#include <silk/stages/parser.h>

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <silk/stages/pipeline.h>
#include <silk/syntax/tree.h>

namespace silk {

const std::unordered_map<TokenKind, Parser::Rule> Parser::rules = {
  // GROUPINGS --------------------------------

  {
    TokenKind::SYM_LT,
    {
      .prefix = &Parser::expression_vector,
    },
  },

  {
    TokenKind::SYM_SQ_OPEN,
    {
      .prefix = &Parser::expression_array,
    },
  },

  {
    TokenKind::SYM_RD_OPEN,
    {
      .prefix     = &Parser::expression_tuple,
      .infix      = &Parser::expression_call,
      .precedence = Precedence::CALL,
    },
  },

  {
    TokenKind::SYM_HASH_BRACE,
    {
      .prefix = &Parser::expression_dictionary,
    },
  },

  // UNARY OPS ---------------------------------

  {
    TokenKind::KW_NOT,
    {
      .prefix = &Parser::expression_unary,
    },
  },

  // BINARY OPS ---------------------------------

  {
    TokenKind::SYM_EQUAL_EQUAL,
    {
      .infix      = &Parser::expression_binary,
      .precedence = Precedence::EQUALITY,
    },
  },

  {
    TokenKind::SYM_BANG_EQUAL,
    {
      .infix      = &Parser::expression_binary,
      .precedence = Precedence::EQUALITY,
    },
  },

  {
    TokenKind::SYM_GT,
    {
      .infix      = &Parser::expression_binary,
      .precedence = Precedence::COMPARISON,
    },
  },
  {
    TokenKind::SYM_GT_EQUAL,
    {
      .infix      = &Parser::expression_binary,
      .precedence = Precedence::COMPARISON,
    },
  },

  {
    TokenKind::SYM_LT,
    {
      .infix      = &Parser::expression_binary,
      .precedence = Precedence::COMPARISON,
    },
  },
  {
    TokenKind::SYM_LT_EQUAL,
    {
      .infix      = &Parser::expression_binary,
      .precedence = Precedence::COMPARISON,
    },
  },

  {
    TokenKind::KW_AND,
    {
      .infix      = &Parser::expression_binary,
      .precedence = Precedence::AND,
    },
  },

  {
    TokenKind::KW_OR,
    {
      .infix      = &Parser::expression_binary,
      .precedence = Precedence::OR,
    },
  },

  {
    TokenKind::SYM_MINUS,
    {
      .prefix     = &Parser::expression_unary,
      .infix      = &Parser::expression_binary,
      .precedence = Precedence::TERM,
    },
  },
  {
    TokenKind::SYM_PLUS,
    {
      .prefix     = &Parser::expression_unary,
      .infix      = &Parser::expression_binary,
      .precedence = Precedence::TERM,
    },
  },
  {
    TokenKind::SYM_STAR,
    {
      .infix      = &Parser::expression_binary,
      .precedence = Precedence::FACTOR,
    },
  },
  {
    TokenKind::SYM_SLASH,
    {
      .infix      = &Parser::expression_binary,
      .precedence = Precedence::FACTOR,
    },
  },
  {
    TokenKind::SYM_STAR_STAR,
    {
      .infix      = &Parser::expression_binary,
      .precedence = Precedence::POWER,
    },
  },
  {
    TokenKind::SYM_SLASH_SLASH,
    {
      .infix      = &Parser::expression_binary,
      .precedence = Precedence::FACTOR,
    },
  },
  {
    TokenKind::SYM_PERC,
    {
      .infix      = &Parser::expression_binary,
      .precedence = Precedence::FACTOR,
    },
  },

  {
    TokenKind::SYM_EQUAL,
    {
      .infix      = &Parser::expression_assignment,
      .precedence = Precedence::ASSIGNMENT,
    },
  },

  {
    TokenKind::SYM_DOT,
    {
      .infix      = &Parser::expression_binary,
      .precedence = Precedence::CALL,
    },
  },

  {
    TokenKind::SYM_PIPE,
    {
      .infix      = &Parser::expression_binary,
      .precedence = Precedence::POWER,
    },
  },

  {
    TokenKind::SYM_DOT_DOT,
    {
      .infix      = &Parser::expression_range,
      .precedence = Precedence::ASSIGNMENT,
    },
  },

  // LITERALS ------------------------------------

  {
    TokenKind::IDENTIFIER,
    {
      .prefix = &Parser::expression_identifier,
    },
  },

  {
    TokenKind::LITERAL_NAT,
    {
      .prefix = &Parser::expression_literal,
    },
  },

  {
    TokenKind::LITERAL_INT,
    {
      .prefix = &Parser::expression_literal,
    },
  },

  {
    TokenKind::LITERAL_REAL,
    {
      .prefix = &Parser::expression_literal,
    },
  },

  {
    TokenKind::LITERAL_CHAR,
    {
      .prefix = &Parser::expression_literal,
    },
  },

  {
    TokenKind::LITERAL_STRING,
    {
      .prefix = &Parser::expression_string,
    },
  },

  {
    TokenKind::BOOL_TRUE,
    {
      .prefix = &Parser::expression_literal,
    },
  },

  {
    TokenKind::BOOL_FALSE,
    {
      .prefix = &Parser::expression_literal,
    },
  },

  {
    TokenKind::KEY_VOID,
    {
      .prefix = &Parser::expression_void,
    },
  },

  {
    TokenKind::KW_RETURN,
    {
      .prefix = &Parser::expression_continuation,
    },
  },

  {
    TokenKind::KEY_PI,
    {
      .prefix = &Parser::expression_literal,
    },
  },

  {
    TokenKind::KEY_TAU,
    {
      .prefix = &Parser::expression_literal,
    },
  },

  {
    TokenKind::KEY_EUL,
    {
      .prefix = &Parser::expression_literal,
    },
  },

  {
    TokenKind::KW_FUN,
    {
      .prefix = &Parser::expression_lambda,
    },
  },
};

inline auto Parser::eof() const -> bool {
  return peek().kind == TokenKind::TOK_END;
}

inline auto Parser::advance() -> Token & {
  _tokens.push_back(_scanner->scan());
  return *(++_tokens.rbegin());
}

inline auto Parser::previous() const -> const Token & {
  return *(++_tokens.rbegin());
}

inline auto Parser::peek() const -> const Token & {
  return *_tokens.rbegin();
}

inline auto Parser::must_match(TokenKind kind, std::string msg) const -> void {
  if (!match(kind)) throw report<LocError>(previous().location, msg);
}

inline auto Parser::must_consume(TokenKind kind, std::string msg) -> void {
  if (!consume(kind)) throw report<LocError>(previous().location, msg);
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

auto Parser::precendece(Precedence prec) -> std::unique_ptr<st::Node> {
  auto rule = get_rule(peek());

  if (!rule) throw report<LocError>(peek().location, "rule not found");

  auto prefix_fn = rule->get().prefix;
  auto prefix    = (this->*prefix_fn)();

  for (auto rule = get_rule(peek());
       rule && rule->get().infix && prec <= rule->get().precedence;
       rule = get_rule(peek())) {
    prefix = std::move((this->*(rule->get().infix))(std::move(prefix)));
  }

  return std::move(prefix);
}

auto Parser::parse_identifier() -> std::string_view {
  must_match(TokenKind::IDENTIFIER, "expected a name");
  return advance().lexeme;
}

auto Parser::parse_package() -> std::string_view {
  must_match(TokenKind::LITERAL_STRING, "expected a package name");
  auto &pkg_str = advance().lexeme;

  if (pkg_str.size() <= 2) {
    throw report<LocError>(previous().location, "invalid package string");
  }

  return std::string_view{pkg_str}.substr(1, pkg_str.size() - 2);
}

auto Parser::parse_typing() -> st::Typing {
  // TODO
  if (!consume(TokenKind::SYM_COLON_COLON)) {
    // no typing
    return nullptr;
  }

  // typing!
  parse_identifier();
  return nullptr;
}

auto Parser::parse_typed_fields(TokenKind end, TokenKind delim)
  -> st::TypedFields {
  auto fields = st::TypedFields{};

  while (!consume(end)) {
    fields.emplace_back(parse_identifier(), parse_typing());
    if (!match(end)) must_consume(delim, "expected a delimiter");
  }

  return fields;
}

auto Parser::declaration() -> std::unique_ptr<st::Node> {
  switch (peek().kind) {
    case TokenKind::KW_MAIN: return declaration_main();
    case TokenKind::KW_PKG: return declaration_package();
    case TokenKind::KW_USE: return declaration_import();
    case TokenKind::KW_FUN: return declaration_function();
    case TokenKind::KW_ENUM: return declaration_enum();
    case TokenKind::KW_OBJ: return declaration_object();
    case TokenKind::KW_DLL: return declaration_library();
    case TokenKind::KW_MACRO: return declaration_macro();
    default: return statement();
  }
}

auto Parser::declaration_main() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::KW_MAIN, "expected `main`");
  must_consume(TokenKind::SYM_SEMICOLON, "expected `;`");
  return make_node<st::ModuleMain>();
}

auto Parser::declaration_package() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::KW_PKG, "expected `pkg`");
  auto package = parse_package();
  must_consume(TokenKind::SYM_SEMICOLON, "expected `;`");
  return make_node<st::ModuleDeclaration>(package);
}

auto Parser::declaration_import() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::KW_USE, "expected `use`");

  auto imports = std::vector<std::string_view>{};

  if (consume(TokenKind::SYM_SLASH)) {
    do {
      imports.push_back(parse_identifier());
    } while (consume(TokenKind::SYM_COMMA));
  }

  must_consume(TokenKind::SYM_SEMICOLON, "expected `;`");
  return make_node<st::ModuleImport>(parse_package(), std::move(imports));
}

auto Parser::declaration_function() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::KW_FUN, "expected `fun`");

  auto name = parse_identifier();

  auto params =
    consume(TokenKind::SYM_RD_OPEN)
      ? parse_typed_fields(TokenKind::SYM_RD_CLOSE, TokenKind::SYM_COMMA)
      : st::TypedFields{};

  auto return_type = parse_typing();

  auto body = consume(TokenKind::SYM_FATARROW)
                ? make_node<st::StatementReturn>(nullptr, expression())
                : statement_block();

  auto lambda =
    make_node<st::ExpressionLambda>(std::move(params), std::move(body));

  return make_node<st::DeclarationFunction>(std::move(name), std::move(lambda));
}

auto Parser::declaration_enum() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::KW_ENUM, "expected `enum`");
  return make_node<st::DeclarationEnum>(parse_identifier());
}

auto Parser::declaration_object() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::KW_OBJ, "expected `obj`");
  return make_node<st::DeclarationObject>(parse_identifier());
}

auto Parser::declaration_library() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::KW_DLL, "expected `dll`");
  // todo
  return make_node<st::DeclarationDynamicLibrary>(parse_identifier());
}

auto Parser::declaration_macro() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::KW_OBJ, "expected `macro`");
  return make_node<st::DeclarationMacro>(parse_identifier());
}

auto Parser::statement() -> std::unique_ptr<st::Node> {
  switch (peek().kind) {
    case TokenKind::SYM_SEMICOLON: return statement_empty();

    case TokenKind::SYM_SQ_OPEN: return statement_block();
    case TokenKind::SYM_DOLLAR_BRACE: return statement_circuit();

    case TokenKind::KW_DEF: [[fallthrough]];
    case TokenKind::KW_LET: return statement_variable();
    case TokenKind::KW_CONST: return statement_constant();

    case TokenKind::KW_RETURN: return statement_return();
    case TokenKind::KW_SWITCH: return statement_switch();
    case TokenKind::KW_CONTINUE: [[fallthrough]];
    case TokenKind::KW_BREAK: return statement_itercontrol();

    case TokenKind::KW_IF: return statement_if();
    case TokenKind::KW_WHILE: return statement_while();
    case TokenKind::KW_LOOP: return statement_loop();
    case TokenKind::KW_FOR: return statement_for();
    case TokenKind::KW_FOREACH: return statement_foreach();
    case TokenKind::KW_MATCH: return statement_match();

    default: return statement_expression();
  }
}

auto Parser::statement_empty() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::SYM_SEMICOLON, "expected `;`");
  return make_node<st::StatementEmpty>();
}

auto Parser::statement_expression() -> std::unique_ptr<st::Node> {
  auto stmt = make_node<st::StatementExpression>(expression());
  must_consume(TokenKind::SYM_SEMICOLON, "expected `;`");
  return stmt;
}

auto Parser::statement_block() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::SYM_BR_OPEN, "expected `{`");

  auto body = std::vector<st::Node>{};
  while (!consume(TokenKind::SYM_BR_CLOSE)) {
    body.emplace_back(std::move(*statement()));
  }

  return make_node<st::StatementBlock>(std::move(body));
}

auto Parser::statement_circuit() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::SYM_DOLLAR_BRACE, "expected `${`");

  auto body = std::vector<std::pair<std::string_view, st::Node>>{};

  while (!consume(TokenKind::SYM_BR_CLOSE)) {
    auto label = parse_identifier();
    must_consume(TokenKind::SYM_COLON, "expected `:`");
    body.push_back({std::move(label), std::move(*statement())});
  }

  return make_node<st::StatementCircuit>(std::move(body));
}

auto Parser::statement_variable() -> std::unique_ptr<st::Node> {
  if (!match(TokenKind::KW_LET, TokenKind::KW_DEF)) {
    throw report<LocError>(peek().location, "expected `let` or `def`");
  }

  auto kind   = static_cast<st::StatementVariable::Kind>(advance().kind);
  auto name   = parse_identifier();
  auto typing = parse_typing();

  must_consume(TokenKind::SYM_EQUAL, "expected `=` after variable declaration");

  auto init = expression();

  must_consume(TokenKind::SYM_SEMICOLON, "expected `;`");
  return make_node<st::StatementVariable>(
    std::move(name), std::move(init), kind);
}

auto Parser::statement_constant() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::KW_CONST, "expected `const`");

  auto name   = parse_identifier();
  auto typing = parse_typing();

  must_consume(TokenKind::SYM_EQUAL, "expected `=` after variable declaration");

  auto init = expression();

  must_consume(TokenKind::SYM_SEMICOLON, "expected `;`");
  return make_node<st::StatementConstant>(std::move(name), std::move(init));
}

auto Parser::statement_return() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::KW_RETURN, "expected `return`");

  auto cont = static_cast<std::unique_ptr<st::Node>>(nullptr);

  if (consume(TokenKind::SYM_SQ_OPEN)) {
    cont = expression();
    must_consume(TokenKind::SYM_SQ_CLOSE, "expected `]`");
  }

  auto value = static_cast<std::unique_ptr<st::Node>>(nullptr);

  if (!match(TokenKind::SYM_SEMICOLON)) value = expression();
  must_consume(TokenKind::SYM_SEMICOLON, "expected `;`");

  return make_node<st::StatementReturn>(std::move(cont), std::move(value));
}

auto Parser::statement_switch() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::KW_SWITCH, "expected `switch`");
  auto label = parse_identifier();
  must_consume(TokenKind::SYM_SEMICOLON, "expected `;`");
  return make_node<st::StatementSwitch>(std::move(label));
}

auto Parser::statement_itercontrol() -> std::unique_ptr<st::Node> {
  if (!match(TokenKind::KW_BREAK, TokenKind::KW_CONTINUE)) {
    throw report<LocError>(peek().location, "expected `break` or `continue`");
  }

  auto kind = static_cast<st::StatementIterationControl::Kind>(advance().kind);

  must_consume(TokenKind::SYM_SEMICOLON, "expected `;`");
  return make_node<st::StatementIterationControl>(kind);
}

auto Parser::statement_if() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::KW_IF, "expected `if`");
  must_consume(TokenKind::SYM_RD_OPEN, "expected `(`");

  auto cond = expression();

  must_consume(TokenKind::SYM_RD_CLOSE, "expected `)`");

  auto conseq = statement();
  auto altern = consume(TokenKind::KW_ELSE) ? statement() : nullptr;

  return make_node<st::StatementIf>(
    std::move(cond), std::move(conseq), std::move(altern));
}

auto Parser::statement_while() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::KW_FOR, "expected `while`");

  must_consume(TokenKind::SYM_RD_OPEN, "expected `(`");
  auto cond = expression();
  must_consume(TokenKind::SYM_RD_CLOSE, "expected `)`");

  return make_node<st::StatementWhile>(std::move(cond), statement());
}

auto Parser::statement_loop() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::KW_LOOP, "expected `loop`");
  return make_node<st::StatementLoop>(statement());
}

auto Parser::statement_for() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::KW_FOR, "expected `for`");

  must_consume(TokenKind::SYM_RD_OPEN, "expected `(`");
  auto init = statement();

  auto cond = expression();
  must_consume(TokenKind::SYM_SEMICOLON, "expected `;`");

  auto incr = expression();
  must_consume(TokenKind::SYM_RD_CLOSE, "expected `)`");

  auto body = statement();

  return make_node<st::StatementFor>(
    std::move(init), std::move(cond), std::move(incr), std::move(body));
}

auto Parser::statement_foreach() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::KW_FOREACH, "expected `foreach`");
  must_consume(TokenKind::SYM_RD_OPEN, "expected `(`");

  if (!match(TokenKind::KW_LET, TokenKind::KW_DEF)) {
    throw report<LocError>(peek().location, "expected `let` or `def`");
  }

  auto iter_kind = static_cast<st::StatementForeach::IterKind>(advance().kind);
  auto iter      = parse_identifier();

  must_consume(TokenKind::SYM_LT_ARROW, "expected `<-`");
  auto collec = expression();
  must_consume(TokenKind::SYM_RD_CLOSE, "expected `)`");

  auto body = statement();

  return make_node<st::StatementForeach>(
    iter_kind, std::move(iter), std::move(collec), std::move(body));
}

auto Parser::statement_match() -> std::unique_ptr<st::Node> {
  must_match(TokenKind::KW_MATCH, "expected match");
  // TODO: implement
  return make_node<st::StatementMatch>();
}

auto Parser::expression() -> std::unique_ptr<st::Node> {
  if (eof()) {
    throw report<LocError>(previous().location, "expected expression");
  }

  return precendece(Precedence::ASSIGNMENT);
}

auto Parser::expression_identifier() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::IDENTIFIER, "expected identifier");
  const auto name = previous().lexeme;
  return make_node<st::ExpressionIdentifier>(std::move(name));
}

auto Parser::expression_void() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::KEY_VOID, "expected `void`");
  return make_node<st::ExpressionVoid>();
}

auto Parser::expression_continuation() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::KW_RETURN, "expected `return`");
  return make_node<st::ExpressionContinuation>();
}

auto Parser::expression_literal() -> std::unique_ptr<st::Node> {
  switch (advance().kind) {
    case TokenKind::BOOL_TRUE: return make_node<st::ExpressionBool>(true);
    case TokenKind::BOOL_FALSE: return make_node<st::ExpressionBool>(false);

    case TokenKind::KEY_PI:
      return make_node<st::ExpressionRealKeyword>(
        st::ExpressionRealKeyword::Kind::PI);

    case TokenKind::KEY_TAU:
      return make_node<st::ExpressionRealKeyword>(
        st::ExpressionRealKeyword::Kind::TAU);

    case TokenKind::KEY_EUL:
      return make_node<st::ExpressionRealKeyword>(
        st::ExpressionRealKeyword::Kind::EULER);

    case TokenKind::LITERAL_NAT:
      return make_node<st::ExpressionNat>(std::stoull(previous().lexeme));

    case TokenKind::LITERAL_INT:
      return make_node<st::ExpressionInt>(std::stoll(previous().lexeme));

    case TokenKind::LITERAL_REAL:
      return make_node<st::ExpressionReal>(std::stod(previous().lexeme));

    case TokenKind::LITERAL_CHAR:
      return make_node<st::ExpressionChar>(
        static_cast<char32_t>(previous().lexeme.at(0)));

    default: throw report<LocError>(previous().location, "expected literal");
  }
}

auto Parser::expression_string() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::LITERAL_STRING, "expected string");

  auto &raw_value = previous().lexeme;

  if (raw_value.size() < 2) {
    throw report<LocError>(previous().location, "invalid string");
  }

  auto parsed = raw_value.substr(1, raw_value.size() - 2);
  // todo parse it

  return make_node<st::ExpressionString>(raw_value, parsed);
}

auto Parser::expression_unary() -> std::unique_ptr<st::Node> {
  if (!match(TokenKind::KW_NOT, TokenKind::SYM_MINUS)) {
    throw report<LocError>(peek().location, "expected `not` or `-`");
  }

  auto kind    = static_cast<st::ExpressionUnaryOp::Kind>(advance().kind);
  auto operand = precendece(Precedence::UNARY);
  return make_node<st::ExpressionUnaryOp>(std::move(operand), kind);
}

auto Parser::expression_binary(std::unique_ptr<st::Node> &&left)
  -> std::unique_ptr<st::Node> {
  auto tok   = advance();
  auto kind  = static_cast<st::ExpressionBinaryOp::Kind>(tok.kind);
  auto rule  = get_rule(tok);
  auto right = precendece(higher(rule->get().precedence));

  return make_node<st::ExpressionBinaryOp>(
    std::move(left), std::move(right), kind);
}

auto Parser::expression_tuple() -> std::unique_ptr<st::Node> {
  auto contents = std::vector<st::Node>{};

  must_consume(TokenKind::SYM_RD_OPEN, "expected `(`");

  while (!consume(TokenKind::SYM_RD_CLOSE)) {
    contents.push_back(std::move(*expression()));

    if (!match(TokenKind::SYM_RD_CLOSE)) {
      must_consume(TokenKind::SYM_COMMA, "expected `,`");
    }
  }

  if (contents.size() == 1) {
    return std::make_unique<st::Node>(std::move(contents.back()));
  } else {
    return make_node<st::ExpressionTuple>(std::move(contents));
  }
}

auto Parser::expression_range(std::unique_ptr<st::Node> &&left)
  -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::SYM_DOT_DOT, "expected `..`");
  auto right = expression();
  return make_node<st::ExpressionRange>(std::move(left), std::move(right));
}

auto Parser::expression_vector() -> std::unique_ptr<st::Node> {
  auto contents = std::vector<st::Node>{};

  must_consume(TokenKind::SYM_LT, "expected `<`");

  while (!consume(TokenKind::SYM_GT)) {
    contents.push_back(std::move(*expression()));

    if (!match(TokenKind::SYM_GT)) {
      must_consume(TokenKind::SYM_COMMA, "expected `,`");
    }
  }

  return make_node<st::ExpressionVector>(std::move(contents));
}

auto Parser::expression_array() -> std::unique_ptr<st::Node> {
  auto contents = std::vector<st::Node>{};

  must_consume(TokenKind::SYM_SQ_OPEN, "expected `[`");

  while (!consume(TokenKind::SYM_SQ_CLOSE)) {
    contents.push_back(std::move(*expression()));

    if (!match(TokenKind::SYM_SQ_CLOSE)) {
      must_match(TokenKind::SYM_COMMA, "expected `,`");
    }
  }

  return make_node<st::ExpressionArray>(std::move(contents));
}

auto Parser::expression_dictionary() -> std::unique_ptr<st::Node> {
  auto contents = std::vector<std::pair<st::Node, st::Node>>{};

  must_consume(TokenKind::SYM_HASH_BRACE, "expected `#{`");

  while (!consume(TokenKind::SYM_BR_CLOSE)) {
    auto key = expression();

    must_consume(TokenKind::SYM_COLON, "expected `:`");

    auto value = expression();

    contents.push_back({std::move(*key), std::move(*value)});

    if (!match(TokenKind::SYM_BR_CLOSE)) {
      must_match(TokenKind::SYM_COMMA, "expected `,`");
    }
  }

  return make_node<st::ExpressionDictionary>(std::move(contents));
}

auto Parser::expression_assignment(std::unique_ptr<st::Node> &&target)
  -> std::unique_ptr<st::Node> {
  if (!match(
        TokenKind::SYM_EQUAL,
        TokenKind::SYM_PLUS_EQUAL,
        TokenKind::SYM_MINUS_EQUAL,
        TokenKind::SYM_SLASH_EQUAL,
        TokenKind::SYM_SLASH_SLASH_EQUAL,
        TokenKind::SYM_STAR_EQUAL,
        TokenKind::SYM_STAR_STAR_EQUAL)) {
    throw report<LocError>(peek().location, "expected assignment");
  }

  auto kind = static_cast<st::ExpressionAssignment::Kind>(advance().kind);

  return make_node<st::ExpressionAssignment>(
    std::move(target), expression(), kind);
}

auto Parser::expression_call(std::unique_ptr<st::Node> &&target)
  -> std::unique_ptr<st::Node> {
  auto args = std::vector<st::Node>{};

  must_consume(TokenKind::SYM_RD_OPEN, "expected `(`");

  do {
    args.push_back(std::move(*expression()));
  } while (consume(TokenKind::SYM_COMMA));

  must_consume(TokenKind::SYM_RD_OPEN, "expected `)`");

  return make_node<st::ExpressionCall>(std::move(target), std::move(args));
}

auto Parser::expression_lambda() -> std::unique_ptr<st::Node> {
  must_consume(TokenKind::KW_FUN, "expected `fun`");

  auto params =
    consume(TokenKind::SYM_RD_OPEN)
      ? parse_typed_fields(TokenKind::SYM_RD_CLOSE, TokenKind::SYM_COMMA)
      : st::TypedFields{};

  auto return_type = parse_typing();

  auto body = consume(TokenKind::SYM_FATARROW)
                ? make_node<st::StatementReturn>(nullptr, expression())
                : statement();

  return make_node<st::ExpressionLambda>(std::move(params), std::move(body));
}

auto Parser::execute(Source &&source) noexcept -> Module {
  _scanner.emplace(source.source);
  _tokens = std::vector<Token>{_scanner->scan()};

  auto tree = std::vector<std::unique_ptr<st::Node>>{};

  while (!eof()) {
    try {
      tree.emplace_back(declaration());
    } catch (...) {
      while (!eof() && !consume(TokenKind::SYM_SEMICOLON)) {
        advance();
      }
    }
  }

  return Module{
    .path   = std::move(source.path),
    .tokens = std::move(_tokens),
    .tree   = std::move(tree),
  };
}

} // namespace silk
