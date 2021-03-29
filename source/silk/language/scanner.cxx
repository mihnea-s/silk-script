#include <silk/language/scanner.h>

#include <cctype>
#include <sstream>
#include <unordered_map>

#include <silk/language/token.h>

namespace silk {

const std::unordered_map<std::string_view, TokenKind> Scanner::keywords = {
  {"main", TokenKind::KW_MAIN},
  {"pkg", TokenKind::KW_PKG},
  {"use", TokenKind::KW_USE},
  {"fun", TokenKind::KW_FUN},
  {"enum", TokenKind::KW_ENUM},
  {"obj", TokenKind::KW_OBJ},
  {"priv", TokenKind::KW_PRIV},
  {"dll", TokenKind::KW_DLL},
  {"macro", TokenKind::KW_MACRO},

  {"let", TokenKind::KW_LET},
  {"def", TokenKind::KW_DEF},
  {"const", TokenKind::KW_CONST},
  {"return", TokenKind::KW_RETURN},
  {"switch", TokenKind::KW_SWITCH},
  {"break", TokenKind::KW_BREAK},
  {"continue", TokenKind::KW_CONTINUE},

  {"if", TokenKind::KW_IF},
  {"else", TokenKind::KW_ELSE},
  {"while", TokenKind::KW_WHILE},
  {"loop", TokenKind::KW_LOOP},
  {"for", TokenKind::KW_FOR},
  {"foreach", TokenKind::KW_FOREACH},
  {"match", TokenKind::KW_MATCH},
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

auto Scanner::peek() const noexcept -> int {
  return _input.peek();
}

auto Scanner::advance() noexcept -> int {
  auto next = _input.get();

  if (next == '\n') {
    _location.first++;
    _location.second = 0;
  } else {
    _location.second++;
  }

  return next;
}

auto Scanner::compound(char c, TokenKind m, TokenKind n) noexcept -> Token {
  if (peek() == c) {
    advance();
    return make_token(m);
  }

  return make_token(n);
}

auto Scanner::scan_comment() noexcept -> Token {
  auto c   = EOF;
  auto oss = std::ostringstream{};

  while ((c = advance()) != EOF && c != '\n') {
    oss << (char)c;
  }

  return make_token(TokenKind::COMMENT, oss.str());
}

auto Scanner::scan_char() noexcept -> Token {
  auto character = (char)advance();
  return make_token(TokenKind::LITERAL_CHAR, std::string{character});
}

auto Scanner::scan_string() noexcept -> Token {
  auto c   = EOF;
  auto oss = std::ostringstream{};

  oss << '\'';

  while ((c = advance()) != EOF && c != '\'') {
    oss << (char)c;
  }

  oss << '\'';

  return make_token(TokenKind::LITERAL_STRING, oss.str());
}

auto Scanner::scan_number(int first) noexcept -> Token {
  auto negative = first == '-';
  auto dotted   = first == '.';

  auto oss = std::ostringstream{};

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

  auto kind = dotted     ? TokenKind::LITERAL_REAL
              : negative ? TokenKind::LITERAL_INT
                         : TokenKind::LITERAL_NAT;

  return make_token(kind, oss.str());
}

auto Scanner::scan_identifier(int first) -> std::string {
  auto oss = std::ostringstream{};
  oss << (char)first;

  while (peek() != EOF && std::isalnum(peek())) {
    oss << (char)advance();
  }

  return oss.str();
}

auto Scanner::make_token(TokenKind kind) const noexcept -> Token {
  return make_token(kind, std::string{});
}

auto Scanner::make_token(TokenKind kind, std::string lexeme) const noexcept
  -> Token {
  return Token{
    .kind     = kind,
    .lexeme   = lexeme,
    .location = _location,
  };
}

auto Scanner::scan() noexcept -> Token {
  while (iswspace(peek())) {
    advance();
  }

  auto c = advance();

  switch (c) {
    case EOF: {
      return make_token(TokenKind::TOK_END);
    }

    case '"': {
      return scan_char();
    }

    case '\'': {
      return scan_string();
    }

    case '#': {
      if (peek() == '{') {
        advance();
        return make_token(TokenKind::SYM_HASH_BRACE);
      }

      return scan_comment();
    }

    case ',': return make_token(TokenKind::SYM_COMMA);
    case ';': return make_token(TokenKind::SYM_SEMICOLON);
    case '(': return make_token(TokenKind::SYM_RD_OPEN);
    case ')': return make_token(TokenKind::SYM_RD_CLOSE);
    case '[': return make_token(TokenKind::SYM_SQ_OPEN);
    case ']': return make_token(TokenKind::SYM_SQ_CLOSE);
    case '{': return make_token(TokenKind::SYM_BR_OPEN);
    case '}': return make_token(TokenKind::SYM_BR_CLOSE);

    case '+': {
      switch (peek()) {
        case '+':
          advance();
          return compound(
            '=', TokenKind::SYM_PLUS_PLUS_EQUAL, TokenKind::SYM_PLUS_PLUS);

        case '=': advance(); return make_token(TokenKind::SYM_PLUS_EQUAL);
        default: return make_token(TokenKind::SYM_PLUS);
      }
    }

    case '-': {
      if (std::isdigit(peek())) return scan_number('-');

      switch (peek()) {
        case '-':
          advance();
          return compound(
            '=', TokenKind::SYM_MINUS_MINUS_EQUAL, TokenKind::SYM_MINUS_MINUS);

        case '=': advance(); return make_token(TokenKind::SYM_MINUS_EQUAL);
        case '>': advance(); return make_token(TokenKind::SYM_ARROW);
        default: return make_token(TokenKind::SYM_MINUS);
      }
    }

    case '/': {
      switch (peek()) {
        case '/':
          advance();
          return compound(
            '=', TokenKind::SYM_SLASH_SLASH_EQUAL, TokenKind::SYM_SLASH_SLASH);

        case '=': advance(); return make_token(TokenKind::SYM_SLASH_EQUAL);
        default: return make_token(TokenKind::SYM_SLASH);
      }
    }

    case '*': {
      switch (peek()) {
        case '*':
          advance();
          return compound(
            '=', TokenKind::SYM_STAR_STAR_EQUAL, TokenKind::SYM_STAR_STAR);

        case '=': advance(); return make_token(TokenKind::SYM_STAR_EQUAL);
        default: return make_token(TokenKind::SYM_STAR);
      }
    }

    case '%': {
      switch (peek()) {
        case '%':
          advance();
          return compound(
            '=', TokenKind::SYM_PERC_PERC_EQUAL, TokenKind::SYM_PERC_PERC);

        case '=': advance(); return make_token(TokenKind::SYM_PERC_EQUAL);
        default: return make_token(TokenKind::SYM_PERC);
      }
    }

    case '=': {
      switch (peek()) {
        case '=':
          advance();
          return compound(
            '=', TokenKind::SYM_EQUAL_EQUAL_EQUAL, TokenKind::SYM_EQUAL_EQUAL);

        case '>': advance(); return make_token(TokenKind::SYM_FATARROW);
        default: return make_token(TokenKind::SYM_EQUAL);
      }
    }

    case '!': {
      switch (peek()) {
        case '!':
          advance();
          return compound(
            '=', TokenKind::SYM_BANG_BANG_EQUAL, TokenKind::SYM_BANG_BANG);

        case '=': advance(); return make_token(TokenKind::SYM_BANG_EQUAL);
        default: return make_token(TokenKind::SYM_BANG);
      }
    }

    case '>': {
      switch (peek()) {
        case '>':
          advance();
          switch (peek()) {
            case '>':
              advance();
              return compound(
                '=', TokenKind::SYM_GT_GT_GT_EQUAL, TokenKind::SYM_GT_GT_GT);

            case '=': advance(); return make_token(TokenKind::SYM_GT_GT_EQUAL);
            default: return make_token(TokenKind::SYM_GT_GT);
          };

        case '=': advance(); return make_token(TokenKind::SYM_GT_EQUAL);
        default: return make_token(TokenKind::SYM_GT);
      }
    }

    case '<': {
      switch (peek()) {
        case '<':
          advance();
          switch (peek()) {
            case '<':
              advance();
              return compound(
                '=', TokenKind::SYM_LT_LT_LT_EQUAL, TokenKind::SYM_LT_LT_LT);

            case '=': advance(); return make_token(TokenKind::SYM_LT_LT_EQUAL);
            default: return make_token(TokenKind::SYM_LT_LT);
          };

        case '=': advance(); return make_token(TokenKind::SYM_LT_EQUAL);
        case '-': advance(); return make_token(TokenKind::SYM_LT_ARROW);
        default: return make_token(TokenKind::SYM_LT);
      }
    }

    case '&': {
      return compound('&', TokenKind::SYM_AMP_AMP, TokenKind::SYM_AMP);
    }

    case '|': {
      return compound('|', TokenKind::SYM_PIPE_PIPE, TokenKind::SYM_PIPE);
    }

    case '~': {
      return compound('~', TokenKind::SYM_TILDE_TILDE, TokenKind::SYM_TILDE);
    }

    case '^': {
      return compound('^', TokenKind::SYM_CARET_CARET, TokenKind::SYM_CARET);
    }

    case '?': {
      return compound('?', TokenKind::SYM_QMARK_QMARK, TokenKind::SYM_QMARK);
    }

    case '@': {
      return compound('@', TokenKind::SYM_ATSIGN_ATSIGN, TokenKind::SYM_ATSIGN);
    }

    case '_': {
      return compound('_', TokenKind::SYM_USCORE_USCORE, TokenKind::SYM_USCORE);
    }

    case '.': {
      if (std::isdigit(peek())) return scan_number(c);

      switch (peek()) {
        case '.':
          advance();
          return compound(
            '.', TokenKind::SYM_DOT_DOT_DOT, TokenKind::SYM_DOT_DOT);

        case '{': advance(); return make_token(TokenKind::SYM_DOT_BRACE);
        default: return make_token(TokenKind::SYM_DOT);
      }
    }

    case ':': {
      switch (peek()) {
        case ':': advance(); return make_token(TokenKind::SYM_COLON_COLON);
        case '{': advance(); return make_token(TokenKind::SYM_COLON_BRACE);
        default: return make_token(TokenKind::SYM_COLON);
      }
    }

    case '$': {
      switch (peek()) {
        case '$': advance(); return make_token(TokenKind::SYM_DOLLAR_DOLLAR);
        case '{': advance(); return make_token(TokenKind::SYM_DOLLAR_BRACE);
        default: return make_token(TokenKind::SYM_DOLLAR);
      }
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

} // namespace silk
