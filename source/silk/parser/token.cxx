#include <cctype>
#include <cwctype>
#include <silk/parser/token.h>
#include <sstream>

const std::unordered_map<std::string_view, TokenKind> TokenScanner::keywords = {
  {"pkg", TokenKind::KW_PKG},
  {"main", TokenKind::KW_MAIN},
  {"use", TokenKind::KW_USE},
  {"extern", TokenKind::KW_EXTERN},

  {"let", TokenKind::KW_LET},
  {"def", TokenKind::KW_DEF},
  {"const", TokenKind::KW_CONST},
  {"fun", TokenKind::KW_FUN},
  {"enum", TokenKind::KW_ENUM},
  {"struct", TokenKind::KW_STRUCT},

  {"if", TokenKind::KW_IF},
  {"else", TokenKind::KW_ELSE},
  {"for", TokenKind::KW_FOR},
  {"match", TokenKind::KW_MATCH},
  {"return", TokenKind::KW_RETURN},
  {"break", TokenKind::KW_BREAK},
  {"continue", TokenKind::KW_CONTINUE},

  {"and", TokenKind::KW_AND},
  {"or", TokenKind::KW_OR},
  {"not", TokenKind::KW_NOT},

  {"true", TokenKind::BOOL_TRUE},
  {"false", TokenKind::BOOL_FALSE},

  {"void", TokenKind::KEY_VOID},
  {"pi", TokenKind::KEY_PI},
  {"tau", TokenKind::KEY_TAU},
  {"eul", TokenKind::KEY_EUL},
};

auto TokenScanner::peek() const noexcept -> int {
  return _input.peek();
}

auto TokenScanner::advance() noexcept -> int {
  auto next = _input.get();

  if (next == '\n') {
    _location.first++;
    _location.second = 0;
  } else {
    _location.second++;
  }

  return next;
}

auto TokenScanner::compound(char c, TokenKind m, TokenKind n) noexcept
  -> Token {
  if (peek() == c) {
    advance();
    return make_token(m);
  }

  return make_token(n);
}

auto TokenScanner::scan_char() noexcept -> Token {
  auto character = (char)advance();
  return make_token(TokenKind::LITERAL_CHAR, std::string{character});
}

auto TokenScanner::scan_string() noexcept -> Token {
  auto c   = EOF;
  auto oss = std::ostringstream{};

  while ((c = advance()) != EOF && c != '\'') {
    oss << (char)c;
  }

  return make_token(TokenKind::LITERAL_STRING, oss.str());
}

auto TokenScanner::scan_number(int first) noexcept -> Token {
  auto dotted = first == '.';
  auto oss    = std::ostringstream{};

  oss << (char)first;

  while (peek() != EOF && std::isdigit(peek())) {
    oss << (char)advance();
  }

  if (!dotted && peek() == '.') {
    dotted = true;

    do {
      oss << (char)advance();
    } while (peek() != EOF && std::isdigit(peek()));
  }

  return make_token(
    dotted ? TokenKind::LITERAL_DOUBLE : TokenKind::LITERAL_INTEGER, oss.str());
}

auto TokenScanner::scan_identifier(int first) -> std::string {
  auto oss = std::ostringstream{};
  oss << (char)first;

  while (peek() != EOF && std::isalnum(peek())) {
    oss << (char)advance();
  }

  return oss.str();
}

auto TokenScanner::make_token(TokenKind kind) const noexcept -> Token {
  return make_token(kind, std::string{});
}

auto TokenScanner::make_token(TokenKind kind, std::string lexeme) const noexcept
  -> Token {
  return Token{
    .kind     = kind,
    .lexeme   = lexeme,
    .location = _location,
  };
}

auto TokenScanner::scan() noexcept -> Token {
  do {
    while (iswspace(peek()))
      advance();

    if (peek() == '#') {
      while (advance() != '\n')
        ;
    }
  } while (iswspace(peek()));

  auto c = advance();

  switch (c) {
    case '~': return make_token(TokenKind::SYM_TILDE);
    case '^': return make_token(TokenKind::SYM_CARET);
    case ',': return make_token(TokenKind::SYM_COMMA);
    case ';': return make_token(TokenKind::SYM_SEMICOLON);
    case '(': return make_token(TokenKind::SYM_LROUND);
    case ')': return make_token(TokenKind::SYM_RROUND);
    case '[': return make_token(TokenKind::SYM_LSQUARE);
    case ']': return make_token(TokenKind::SYM_RSQUARE);
    case '{': return make_token(TokenKind::SYM_LBRACE);
    case '}': return make_token(TokenKind::SYM_RBRACE);

    case '/': {
      return compound('/', TokenKind::SYM_SLASHSLASH, TokenKind::SYM_SLASH);
    }

    case '*': {
      return compound('*', TokenKind::SYM_STARSTAR, TokenKind::SYM_STAR);
    }

    case '%': {
      return compound('%', TokenKind::SYM_PERCPERC, TokenKind::SYM_PERCENT);
    }

    case '=': {
      return compound('=', TokenKind::SYM_EQUALEQUAL, TokenKind::SYM_EQUAL);
    }

    case '!': {
      return compound('=', TokenKind::SYM_BANGEQUAL, TokenKind::SYM_BANG);
    }

    case '>': {
      return compound('>', TokenKind::SYM_GTEQUAL, TokenKind::SYM_GT);
    }

    case '&': {
      return compound('&', TokenKind::SYM_AMPAMP, TokenKind::SYM_AMP);
    }

    case '|': {
      return compound('|', TokenKind::SYM_PIPEPIPE, TokenKind::SYM_PIPE);
    }

    case '?': {
      return compound('?', TokenKind::SYM_QMARKQMARK, TokenKind::SYM_QMARK);
    }

    case ':': {
      return compound(':', TokenKind::SYM_COLONCOLON, TokenKind::SYM_COLON);
    }

    case '@': {
      return compound('@', TokenKind::SYM_ATAT, TokenKind::SYM_AT);
    }

    case '_': {
      return compound('_', TokenKind::SYM_USCOREUSCORE, TokenKind::SYM_USCORE);
    }

    case '+': {
      switch (peek()) {
        case '+': advance(); return make_token(TokenKind::SYM_PLUSPLUS);
        case '=': advance(); return make_token(TokenKind::SYM_PLUSEQUAL);
        default: return make_token(TokenKind::SYM_PLUS);
      }
    }

    case '-': {
      switch (peek()) {
        case '-': advance(); return make_token(TokenKind::SYM_MINUSMINUS);
        case '=': advance(); return make_token(TokenKind::SYM_MINUSEQUAL);
        case '>': advance(); return make_token(TokenKind::SYM_ARROW);
        default: return make_token(TokenKind::SYM_MINUS);
      }
    }

    case '<': {
      switch (peek()) {
        case '=': advance(); return make_token(TokenKind::SYM_LTEQUAL);
        case '-': advance(); return make_token(TokenKind::SYM_RARROW);
        default: return make_token(TokenKind::SYM_LT);
      }
    }

    case EOF: {
      return make_token(TokenKind::TOK_END);
    }

    case '"': {
      return scan_char();
    }

    case '\'': {
      return scan_string();
    }

    case '.': {
      if (std::isdigit(peek())) return scan_number(c);
    }

    default: {
      if (std::isdigit(c)) return scan_number(c);

      auto id = scan_identifier(c);

      if (keywords.find(id) != keywords.end()) {
        return make_token(keywords.at(id));
      } else {
        return make_token(TokenKind::IDENTIFIER, id);
      }
    }
  }
}
