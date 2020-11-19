#include <silk/lexer/lexer.h>

#include <cassert>

#include <silk/lexer/token.h>
#include <silk/util/error.h>

const std::vector<Token>& Lexer::scan(std::istream& in) noexcept {
  // ensure previous run does not affect
  // current output
  _tokens.clear();

  // for every line in the input stream
  while (std::getline(in, current_line)) {
    advance_line();

    for (; column < current_line.size(); advance_column()) {
      auto c = current_line[column];
      switch (c) {
        case '+': {
          switch (peek()) {
            case '+': {
              advance_column();
              add(TokenType::SYM_PLUSPLUS, "++");
              break;
            }
            case '=': {
              advance_column();
              add(TokenType::SYM_PLUSEQUAL, "+=");
              break;
            }
            case '\0':
            default: {
              add(TokenType::SYM_PLUS, "+");
              break;
            }
          }

          break;
        }

        case '-': {
          switch (peek()) {
            case '-': {
              advance_column();
              add(TokenType::SYM_MINUSMINUS, "--");
              break;
            }
            case '=': {
              advance_column();
              add(TokenType::SYM_MINUSEQUAL, "-=");
              break;
            }

            case '>': {
              advance_column();
              add(TokenType::SYM_ARROW, "->");
              break;
            }

            default: {
              add(TokenType::SYM_MINUS, "-");
              break;
            }
          }

          break;
        }

        case '/': {
          compound("//", {TokenType::SYM_SLASH, TokenType::SYM_SLASHSLASH});
          break;
        }

        case '*': {
          compound("**", {TokenType::SYM_STAR, TokenType::SYM_STARSTAR});
          break;
        }

        case '%': {
          compound("%%", {TokenType::SYM_PERCENT, TokenType::SYM_PERCPERC});
          break;
        }

        case '=': {
          compound("==", {TokenType::SYM_EQUAL, TokenType::SYM_EQUALEQUAL});
          break;
        }

        case '!': {
          compound("==", {TokenType::SYM_BANG, TokenType::SYM_BANGEQUAL});
          break;
        }

        case '>': {
          compound(">=", {TokenType::SYM_GT, TokenType::SYM_GTEQUAL});
          break;
        }

        case '<': {
          switch (peek()) {
            case '=': {
              add(TokenType::SYM_LTEQUAL, "<=");
              break;
            }

            case '-': {
              add(TokenType::SYM_RARROW, "<-");
              break;
            }

            default: {
              add(TokenType::SYM_LT, "<");
              break;
            }
          }

          break;
        }

        case '&': {
          compound("&&", {TokenType::SYM_AMP, TokenType::SYM_AMPAMP});
          break;
        }

        case '|': {
          compound("||", {TokenType::SYM_PIPE, TokenType::SYM_PIPEPIPE});
          break;
        }

        case '?': {
          compound("??", {TokenType::SYM_QMARK, TokenType::SYM_QMARKQMARK});
          break;
        }

        case ':': {
          compound("::", {TokenType::SYM_COLON, TokenType::SYM_COLONCOLON});
          break;
        }

        case '@': {
          compound("@@", {TokenType::SYM_AT, TokenType::SYM_ATAT});
          break;
        }

        case '_': {
          compound("__", {TokenType::SYM_USCORE, TokenType::SYM_USCOREUSCORE});
          break;
        }

        case '~': {
          add(TokenType::SYM_TILDE, "~");
          break;
        }

        case '^': {
          add(TokenType::SYM_CARET, "^");
          break;
        }

        case ';': {
          add(TokenType::SYM_SEMICOLON, ";");
          break;
        }

        case '.': {
          if (std::isdigit(peek())) {
            number(true);
          } else {
            compound("..", {TokenType::SYM_DOT, TokenType::SYM_DOTDOT});
          }

          break;
        }

        case ',': {
          add(TokenType::SYM_COMMA, ",");
          break;
        }

        case '#': {
          while (!eol()) {
            advance_column();
          }
          break;
        }

        case '(': {
          add(TokenType::SYM_LROUND, ")");
          break;
        }
        case ')': {
          add(TokenType::SYM_RROUND, "(");
          break;
        }

        case '[': {
          add(TokenType::SYM_LSQUARE, "[");
          break;
        }
        case ']': {
          add(TokenType::SYM_RSQUARE, "]");
          break;
        }

        case '{': {
          add(TokenType::SYM_LBRACE, "{");
          break;
        }
        case '}': {
          add(TokenType::SYM_RBRACE, "}");
          break;
        }

        case '\'': {
          string();
          break;
        }

        default: {

          if (std::iswspace(c)) break;

          if (std::isdigit(c)) {
            number();
            break;
          }

          auto w = word();
          add(keyword(w), std::string {w});
          break;
        }
      }
    }
  }

  return _tokens;
}

inline void Lexer::advance_line() {
  // reset column on a new line
  column = 0;

  line++;
  return;
}

inline void Lexer::advance_column() {
  column++;
  return;
}

inline bool Lexer::next(char c) const {
  if (eol()) return false;
  return current_line[column + 1] == c;
}

inline char Lexer::peek() const {
  // return NULL terminator if end of line
  if (eol()) return '\0';
  return current_line[column + 1];
}

inline bool Lexer::eol() const {
  return column == current_line.size() - 1;
}

inline void
Lexer::compound(std::string_view sv, std::initializer_list<TokenType> types) {
  assert(sv.size() == types.size());

  for (auto i = 0; i < sv.size(); i++) {
    if (peek() != sv.at(i)) {
      add(*(types.begin() + i), std::string {sv.begin(), sv.begin() + i});
      return;
    }
    advance_column();
  }

  add(*(types.end() - 1), std::string {sv});
}

inline bool Lexer::alphanum(char c) const {
  return std::isalnum(c) || c == '_';
}

inline TokenType
Lexer::matchesRest(std::string_view s, std::string_view v, TokenType t) {
  if (s.size() != v.size()) return TokenType::IDENTIFIER;
  for (auto i = 0; i < s.size(); i++) {
    if (s[i] != v[i]) return TokenType::IDENTIFIER;
  }
  return t;
}

inline TokenType Lexer::keyword(std::string_view sv) {
  switch (sv[0]) {

    case 'a': {
      if (sv.size() < 2) return TokenType::IDENTIFIER;
      switch (sv[1]) {
        case 'n': return matchesRest("d", sv.substr(2), TokenType::KW_AND);
        default: return TokenType::IDENTIFIER;
      }
    }

    case 'f': {
      if (sv.size() < 2) return TokenType::IDENTIFIER;
      switch (sv[1]) {
        case 'u': return matchesRest("n", sv.substr(2), TokenType::KW_FUN);
        case 'a':
          return matchesRest("lse", sv.substr(2), TokenType::CNST_FALSE);
        case 'o': return matchesRest("r", sv.substr(2), TokenType::KW_FOR);
        default: return TokenType::IDENTIFIER;
      }
    }

    case 'c': {
      if (sv.size() < 2) return TokenType::IDENTIFIER;
      switch (sv[1]) {
        case 'o': {
          if (sv.size() < 4) return TokenType::IDENTIFIER;
          switch (sv[3]) { // c o n [s]
            case 's': matchesRest("t", sv.substr(4), TokenType::KW_CONST);
            case 't':
              return matchesRest("inue", sv.substr(4), TokenType::KW_CONTINUE);
            default: return TokenType::IDENTIFIER;
          }
        }
        default: return TokenType::IDENTIFIER;
      }
    }

    case 'p': {
      if (sv.size() < 2) return TokenType::IDENTIFIER;
      switch (sv[1]) {
        case 'i': return matchesRest("", sv.substr(2), TokenType::CNST_PI);
        case 'k': return matchesRest("g", sv.substr(2), TokenType::KW_PKG);
        default: return TokenType::IDENTIFIER;
      }
    }

    case 't': {
      if (sv.size() < 3) return TokenType::IDENTIFIER;
      switch (sv[1]) {
        case 'a': return matchesRest("u", sv.substr(2), TokenType::CNST_TAU);
        case 'r': return matchesRest("ue", sv.substr(2), TokenType::CNST_TRUE);
        default: return TokenType::IDENTIFIER;
      }
    }

    case 'e': {
      if (sv.size() < 2) return TokenType::CNST_EUL;
      switch (sv[1]) {
        case 'l': return matchesRest("se", sv.substr(2), TokenType::KW_ELSE);
        case 'x':
          return matchesRest("tern", sv.substr(2), TokenType::KW_EXTERN);
        default: return TokenType::IDENTIFIER;
      }
    }

    case 'm': {
      if (sv.size() < 3 || sv[1] != 'a') return TokenType::IDENTIFIER;
      switch (sv[2]) {
        case 't': return matchesRest("ch", sv.substr(3), TokenType::KW_MATCH);
        case 'i': return matchesRest("n", sv.substr(3), TokenType::KW_MAIN);
        default: return TokenType::IDENTIFIER;
      }
    }

    case 'i': {
      if (sv.size() < 2) return TokenType::IDENTIFIER;
      switch (sv[1]) {
        case 'f': return matchesRest("", sv.substr(2), TokenType::KW_IF);
        default: return TokenType::IDENTIFIER;
      }
    }

    case 'd': {
      if (sv.size() < 2) return TokenType::IDENTIFIER;
      switch (sv[1]) {
        case 'e': return matchesRest("f", sv.substr(2), TokenType::KW_DEF);
        default: return TokenType::IDENTIFIER;
      }
    }

    case 's': return matchesRest("truct", sv.substr(1), TokenType::KW_STRUCT);
    case 'r': return matchesRest("eturn", sv.substr(1), TokenType::KW_RETURN);
    case 'u': return matchesRest("se", sv.substr(1), TokenType::KW_USE);
    case 'l': return matchesRest("et", sv.substr(1), TokenType::KW_LET);
    case 'b': return matchesRest("reak", sv.substr(1), TokenType::KW_BREAK);
    case 'n': return matchesRest("ot", sv.substr(1), TokenType::KW_NOT);
    case 'o': return matchesRest("r", sv.substr(1), TokenType::KW_OR);
    case 'v': return matchesRest("oid", sv.substr(1), TokenType::CNST_VOID);

    default: return TokenType::IDENTIFIER;
  }
}

inline void Lexer::number(bool real) {
  int start = column;

  if (!real && next('.')) {
    real = true;
    advance_column();
  }

  while (!eol() && std::isdigit(peek())) {
    advance_column();
    if (!real && next('.')) {
      real = true;
      advance_column();
    }
  }

  auto num_str = std::string {
    begin(current_line) + start,
    begin(current_line) + column + 1,
  };

  if (real) {
    add(TokenType::LITERAL_DOUBLE, num_str);
  } else {
    add(TokenType::LITERAL_INTEGER, num_str);
  }
}

inline void Lexer::string() {
  // start at first quote
  int start = column;

  // advance until we hit either the
  // end of line or a single quote
  while (!eol() && !next('\'')) {
    advance_column();
  }

  // advance once more to the quote
  advance_column();

  // get the characters in-between the quotes
  auto str = std::string {
    std::begin(current_line) + start + 1,
    std::begin(current_line) + column,
  };

  add(TokenType::LITERAL_STRING, str);
}

inline std::string_view Lexer::word() {
  // start at current character
  int start = column;

  // advance column until we hit either a
  // alpha-numerical character or the end of line
  while (!eol() && alphanum(peek())) {
    advance_column();
  }

  // number of characters in-between + the first character
  std::size_t length = column - start + 1;

  return std::string_view {current_line.data() + start, length};
};
