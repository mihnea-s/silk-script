#include <cstdint>
#include <cstdlib>

#include <vm/chunk.h>
#include <vm/constants.h>
#include <vm/file_exec.h>
#include <vm/mem.h>
#include <vm/opcode.h>

#include <silk/common/error.h>
#include <silk/common/token.h>
#include <silk/compiler/compiler.h>

std::map<TokenType, Compiler::Rule> Compiler::rules = {
  // GROUPING -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=

  {
    TokenType::sym_lround,
    {
      .prefix = &Compiler::expr_grouping,
    },
  },

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-

  // LOGICAL OPERATIONS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  {
    TokenType::sym_bang,
    {
      .prefix = &Compiler::expr_unary,
    },
  },

  {
    TokenType::sym_equalequal,
    {
      .infix = &Compiler::expr_binary,
      .prec  = Precedence::EQUALITY,
    },
  },

  {
    TokenType::sym_bangequal,
    {
      .infix = &Compiler::expr_binary,
      .prec  = Precedence::EQUALITY,
    },
  },

  {
    TokenType::sym_gt,
    {
      .infix = &Compiler::expr_binary,
      .prec  = Precedence::COMPARISON,
    },
  },
  {
    TokenType::sym_gtequal,
    {
      .infix = &Compiler::expr_binary,
      .prec  = Precedence::COMPARISON,
    },
  },

  {
    TokenType::sym_lt,
    {
      .infix = &Compiler::expr_binary,
      .prec  = Precedence::COMPARISON,
    },
  },
  {
    TokenType::sym_ltequal,
    {
      .infix = &Compiler::expr_binary,
      .prec  = Precedence::COMPARISON,
    },
  },

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-

  // ARITHMETIC OPERATIONS -=-=-=-=-=-=-=-=-=-=-=-=-=-=

  {
    TokenType::sym_minus,
    {
      .prefix = &Compiler::expr_unary,
      .infix  = &Compiler::expr_binary,
      .prec   = Precedence::TERM,
    },
  },
  {
    TokenType::sym_plus,
    {
      .infix = &Compiler::expr_binary,
      .prec  = Precedence::TERM,
    },
  },
  {
    TokenType::sym_star,
    {
      .infix = &Compiler::expr_binary,
      .prec  = Precedence::FACTOR,
    },
  },
  {
    TokenType::sym_slash,
    {
      .infix = &Compiler::expr_binary,
      .prec  = Precedence::FACTOR,
    },
  },
  {
    TokenType::sym_starstar,
    {
      .infix = &Compiler::expr_binary,
      .prec  = Precedence::POWER,
    },
  },
  {
    TokenType::sym_slashslash,
    {
      .infix = &Compiler::expr_binary,
      .prec  = Precedence::FACTOR,
    },
  },
  {
    TokenType::sym_percent,
    {
      .infix = &Compiler::expr_binary,
      .prec  = Precedence::FACTOR,
    },
  },

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-

  // LITERALS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  {
    TokenType::literal_int,
    {
      .prefix = &Compiler::literal_integer,
    },
  },

  {
    TokenType::literal_dbl,
    {
      .prefix = &Compiler::literal_double,
    },
  },

  {
    TokenType::kw_true,
    {
      .prefix = &Compiler::literal_bool,
    },
  },

  {
    TokenType::kw_false,
    {
      .prefix = &Compiler::literal_bool,
    },
  },

  {
    TokenType::literal_str,
    {
      .prefix = &Compiler::literal_string,
    },
  },

  {
    TokenType::kw_vid,
    {
      .prefix = &Compiler::literal_vid,
    },
  }

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-
};

inline auto Compiler::forward() -> void {
  _tok++;
}

inline auto Compiler::backward() -> void {
  _tok--;
}

inline auto Compiler::advance() -> Token {
  _tok++;
  return previous();
}

inline auto Compiler::previous() const -> Token {
  return *(_tok - 1);
}

inline auto Compiler::current() const -> Token {
  return *_tok;
}

inline auto Compiler::eof() const -> bool {
  return _tok == _end;
}

inline auto Compiler::error_location() const
  -> std::pair<std::uint64_t, std::uint64_t> {
  if (eof()) return previous().location();
  return current().location();
}

inline auto Compiler::throw_error(std::string msg) -> void {
  throw ParsingError {Severity::error, msg, error_location()};
}

inline auto Compiler::should_match(TokenType type, std::string msg) const
  -> void {
  if (!match(type)) {
    throw ParsingError {Severity::warning, msg, error_location()};
  }
}

inline auto Compiler::must_match(TokenType type, std::string msg) const
  -> void {
  if (!match(type)) {
    throw ParsingError {Severity::error, msg, error_location()};
  }
}

inline auto Compiler::should_consume(TokenType type, std::string msg) -> void {
  if (!consume(type)) {
    throw ParsingError {Severity::warning, msg, error_location()};
  }
}

inline auto Compiler::must_consume(TokenType type, std::string msg) -> void {
  if (!consume(type)) {
    throw ParsingError {Severity::error, msg, error_location()};
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
  write_ins(cnk, VM_VAL);
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
  if (eof()) throw_error("expected expression");
  precendece(Precedence::ASSIGNMENT);
}

auto Compiler::expr_unary() -> void {
  auto type = advance().type();

  precendece(Precedence::UNARY);

  switch (type) {
    case TokenType::sym_minus: {
      emit(VM_NEG);
      break;
    }

    case TokenType::sym_bang: {
      emit(VM_NOT);
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
    // arithmetic
    case TokenType::sym_plus: return emit(VM_ADD);
    case TokenType::sym_minus: return emit(VM_SUB);
    case TokenType::sym_star: return emit(VM_MUL);
    case TokenType::sym_slash: return emit(VM_DIV);
    case TokenType::sym_starstar: return emit(VM_POW);
    case TokenType::sym_slashslash: return emit(VM_RIV);
    case TokenType::sym_percent: return emit(VM_MOD);

    // logic
    case TokenType::sym_equalequal: return emit(VM_EQ);
    case TokenType::sym_bangequal: return emit(VM_NEQ);
    case TokenType::sym_gt: return emit(VM_GT);
    case TokenType::sym_gtequal: return emit(VM_GTE);
    case TokenType::sym_lt: return emit(VM_LT);
    case TokenType::sym_ltequal: return emit(VM_LTE);

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
  auto val = Value {
    .type       = T_INT,
    .as.integer = std::atoi(tok.lexeme().c_str()),
  };

  cnst(val);
}

auto Compiler::literal_double() -> void {
  auto tok = advance();
  auto val = Value {
    .type    = T_REAL,
    .as.real = std::atof(tok.lexeme().c_str()),
  };

  cnst(val);
}

auto Compiler::literal_bool() -> void {
  auto booleanValue = advance().type() == TokenType::kw_true;
  emit(booleanValue ? VM_TRU : VM_FAL);
}

auto Compiler::literal_string() -> void {
  auto tok = advance();
  auto str = (char*)memory(NULL, 0, tok.lexeme().size() + 1);
  memcpy(str, tok.lexeme().c_str(), tok.lexeme().size());
  str[tok.lexeme().size()] = '\0';

  auto val = Value {
    .type      = T_STR,
    .as.string = str,
  };

  cnst(val);
}

auto Compiler::literal_vid() -> void {
  advance();
  emit(VM_VID);
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

auto Compiler::compile(Iter begin, Iter end) noexcept -> void {
  free_chunks();
  _chunks.clear();

  this->_tok = begin;
  this->_end = end;

  while (!eof()) {
    try {
      expression();
    } catch (ParsingError& err) {
      _errors.push_back(err);

      while (!eof() && !match(TokenType::sym_semicolon)) {
        forward();
      }
    }
  }
}

auto Compiler::write_to_file(std::string_view file) noexcept -> void {
  const char* err = nullptr;

  Program prog = {
    .len  = _chunks.size(),
    .cnks = _chunks.data(),
  };

  write_file(file.data(), &prog, &err);
  if (err) { _errors.push_back(ParsingError {Severity::error, err, {0, 0}}); }
}