#include <cstdint>
#include <cstdlib>

#include <vm/chunk.h>
#include <vm/constants.h>
#include <vm/opcode.h>

#include <silk/common/token.h>
#include <silk/compiler/compiler.h>

std::map<TokenType, Compiler::Rule> Compiler::rules = {
  // GROUPING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=

  {
    TokenType::sym_lround,
    {
      .prefix  = &Compiler::expr_grouping,
      .infix   = nullptr,
      .postfix = nullptr,
      .prec    = Precedence::ANY,
    },
  },

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-

  // ARITHMETIC OPERATIONS -=-=-=-=-=-=-=-=-=-=-=-=-=-=

  {
    TokenType::sym_minus,
    {
      .prefix  = &Compiler::expr_unary,
      .infix   = &Compiler::expr_binary,
      .postfix = nullptr,
      .prec    = Precedence::TERM,
    },
  },
  {
    TokenType::sym_plus,
    {
      .prefix  = nullptr,
      .infix   = &Compiler::expr_binary,
      .postfix = nullptr,
      .prec    = Precedence::TERM,
    },
  },
  {
    TokenType::sym_star,
    {
      .prefix  = nullptr,
      .infix   = &Compiler::expr_binary,
      .postfix = nullptr,
      .prec    = Precedence::FACTOR,
    },
  },
  {
    TokenType::sym_slash,
    {
      .prefix  = nullptr,
      .infix   = &Compiler::expr_binary,
      .postfix = nullptr,
      .prec    = Precedence::FACTOR,
    },
  },
  {
    TokenType::sym_starstar,
    {
      .prefix  = nullptr,
      .infix   = &Compiler::expr_binary,
      .postfix = nullptr,
      .prec    = Precedence::POWER,
    },
  },
  {
    TokenType::sym_slashslash,
    {
      .prefix  = nullptr,
      .infix   = &Compiler::expr_binary,
      .postfix = nullptr,
      .prec    = Precedence::FACTOR,
    },
  },
  {
    TokenType::sym_percent,
    {
      .prefix  = nullptr,
      .infix   = &Compiler::expr_binary,
      .postfix = nullptr,
      .prec    = Precedence::FACTOR,
    },
  },

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-

  // LITERALS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  {
    TokenType::literal_int,
    {
      .prefix  = &Compiler::literal_integer,
      .infix   = nullptr,
      .postfix = nullptr,
      .prec    = Precedence::ANY,
    },
  },

  {
    TokenType::literal_dbl,
    {
      .prefix  = &Compiler::literal_double,
      .infix   = nullptr,
      .postfix = nullptr,
      .prec    = Precedence::ANY,
    },
  },

  {
    TokenType::literal_str,
    {
      .prefix  = &Compiler::literal_string,
      .infix   = nullptr,
      .postfix = nullptr,
      .prec    = Precedence::ANY,
    },
  },

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-
};

inline auto Compiler::forward() -> void {
  _tok++;
}

inline auto Compiler::backward() -> void {
  _tok--;
}

inline auto Compiler::previous() -> Token {
  return *(_tok - 1);
}

inline auto Compiler::advance() -> Token {
  _tok++;
  return previous();
}

inline auto Compiler::current() const -> Token {
  return *_tok;
}

inline auto Compiler::eof() const -> bool {
  return _tok == _end;
}

inline auto Compiler::throw_error(std::string msg) -> void {
  throw ParsingError {Severity::error, msg, current().location()};
}

inline auto Compiler::should_match(TokenType type, std::string msg) const
  -> void {
  if (!match(type)) {
    throw ParsingError {Severity::warning, msg, current().location()};
  }
}

inline auto Compiler::must_match(TokenType type, std::string msg) const
  -> void {
  if (!match(type)) {
    throw ParsingError {Severity::error, msg, current().location()};
  }
}

inline auto Compiler::should_consume(TokenType type, std::string msg) -> void {
  if (!consume(type)) {
    throw ParsingError {Severity::warning, msg, current().location()};
  }
}

inline auto Compiler::must_consume(TokenType type, std::string msg) -> void {
  if (!consume(type)) {
    throw ParsingError {Severity::error, msg, current().location()};
  }
}

inline auto Compiler::current_chunk() -> Chunk* {
  if (_chunks.empty()) {
    _chunks.emplace_back();
    init_chunk(&_chunks.back());
  }
  return &_chunks.back();
}

auto Compiler::cnst(Value value) -> void {
  auto cnk = current_chunk();
  write_ins(cnk, VAL);
  write_ins(cnk, cnk->constants.len);
  write_constant(&cnk->constants, value);
}

auto Compiler::emit(std::uint8_t byte) -> void {
  write_ins(current_chunk(), byte);
}

auto Compiler::get_rule(TokenType type) -> Rule& {
  if (rules.find(type) == rules.end()) throw_error("fatal compiler error");
  return rules[type];
}

auto Compiler::higher(Precedence prec) -> Precedence {
  if (prec == Precedence::NONE) return prec;
  return static_cast<Precedence>(static_cast<int>(prec) + 1);
}

auto Compiler::lower(Precedence prec) -> Precedence {
  if (prec == Precedence::ANY) return prec;
  return static_cast<Precedence>(static_cast<int>(prec) - 1);
}

auto Compiler::precendece(Precedence prec) -> void {
  auto rule = get_rule(current().type());

  (this->*rule.prefix)();

  while (!eof() && prec <= get_rule(current().type()).prec) {
    auto rule = get_rule(current().type());
    (this->*rule.infix)();
  }
}

auto Compiler::expression() -> void {
  precendece(Precedence::ASSIGNMENT);
}

auto Compiler::expr_unary() -> void {
  auto type = advance().type();

  precendece(Precedence::UNARY);

  switch (type) {
    case TokenType::sym_minus: {
      emit(NEG);
      break;
    }

    default: {
      throw_error("invalid unary operation");
    }
  }
}

auto Compiler::expr_binary() -> void {
  auto type = advance().type();

  auto rule = get_rule(type);
  precendece(higher(rule.prec));

  switch (type) {
    case TokenType::sym_plus: {
      emit(ADD);
      break;
    }

    case TokenType::sym_minus: {
      emit(SUB);
      break;
    }

    case TokenType::sym_star: {
      emit(MUL);
      break;
    }

    case TokenType::sym_slash: {
      emit(DIV);
      break;
    }

    case TokenType::sym_starstar: {
      emit(POW);
      break;
    }

    case TokenType::sym_slashslash: {
      emit(RIV);
      break;
    }

    case TokenType::sym_percent: {
      emit(MOD);
      break;
    }

    default: {
      throw_error("invalid binary expression");
    }
  }
}

auto Compiler::expr_grouping() -> void {
  expression();
  must_consume(TokenType::sym_rround, "expected closing parenthesis `)`");
}

auto Compiler::literal_integer() -> void {
  auto tok = advance();
  auto val = std::atoi(tok.lexeme().c_str());
  cnst(val);
}

auto Compiler::literal_double() -> void {
  auto tok = advance();
  auto val = std::atof(tok.lexeme().c_str());
  cnst(val);
}

auto Compiler::literal_string() -> void {
  auto tok = advance();
  cnst(-1);
}

// error functions

auto Compiler::has_error() const -> bool {
  return !_errors.empty();
}

auto Compiler::clear_errors() -> void {
  _errors.clear();
}

auto Compiler::errors() const -> const std::vector<ParsingError>& {
  return _errors;
}

// public compile function

auto Compiler::compile(Iter begin, Iter end) noexcept -> std::vector<Chunk> {
  _chunks.clear();

  this->_tok = begin;
  this->_end = end;

  expression();

  return std::move(_chunks);
}