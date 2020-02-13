#include <cstdint>
#include <cstdlib>
#include <memory>
#include <vector>

#include <silk/common/ast.h>
#include <silk/common/error.h>
#include <silk/common/parser.h>
#include <silk/common/token.h>

std::unordered_map<TokenType, Parser::Rule> Parser::rules = {
  // GROUPING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=

  {
    TokenType::sym_lround,
    {
      .prefix = &Parser::expr_grouping,
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
};

inline auto Parser::forward() -> void {
  _tok++;
}

inline auto Parser::backward() -> void {
  _tok--;
}

inline auto Parser::advance() -> Token {
  _tok++;
  return previous();
}

inline auto Parser::previous() const -> Token {
  return *(_tok - 1);
}

inline auto Parser::current() const -> Token {
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

auto Parser::get_rule(TokenType type) -> Rule& {
  if (rules.find(type) == rules.end())
    throw report_error(error_location(), "fatal compiler error");
  return rules[type];
}

auto Parser::higher(Precedence prec) -> Precedence {
  if (prec == Precedence::NONE) return prec;
  return static_cast<Precedence>(static_cast<int>(prec) + 1);
}

auto Parser::lower(Precedence prec) -> Precedence {
  if (prec == Precedence::ANY) return prec;
  return static_cast<Precedence>(static_cast<int>(prec) - 1);
}

auto Parser::precendece(Precedence prec) -> ASTNodePtr {
  auto rule = get_rule(current().type());

  auto prefix = (this->*rule.prefix)();

  while (!eof() && prec <= get_rule(current().type()).prec) {
    auto rule = get_rule(current().type());
    prefix    = (this->*rule.infix)(std::move(prefix));
  }

  return prefix;
}

auto Parser::expression() -> ASTNodePtr {
  if (eof()) throw report_error(error_location(), "expected expression");
  return precendece(Precedence::ASSIGNMENT);
}

auto Parser::expr_unary() -> ASTNodePtr {
  auto tok     = advance();
  auto operand = precendece(Precedence::UNARY);
  return make_node<Unary>(tok.type(), std::move(operand));
}

auto Parser::expr_binary(ASTNodePtr left) -> ASTNodePtr {
  auto tok   = advance();
  auto rule  = get_rule(tok.type());
  auto right = precendece(higher(rule.prec));

  return make_node<Binary>(tok.type(), std::move(left), std::move(right));
}

auto Parser::expr_grouping() -> ASTNodePtr {
  must_consume(TokenType::sym_lround, "expected opening parenthesis `)`");
  auto inner = expression();
  must_consume(TokenType::sym_rround, "expected closing parenthesis `)`");

  return std::unique_ptr<Grouping>(new Grouping {
    inner->location,
    ASTNode::Grouping,
    std::move(inner),
  });
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
      program.push_back(std::move(expression()));
    } catch (...) {
      while (!eof() && !match(TokenType::sym_semicolon)) {
        forward();
      }
    }
  }

  return AST {std::move(program)};
}
