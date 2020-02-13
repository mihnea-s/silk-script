#include <cassert>
#include <initializer_list>
#include <istream>
#include <string>
#include <string_view>

#include <silk/common/error.h>
#include <silk/common/lexer.h>
#include <silk/common/token.h>

const Lexer::TokensVector& Lexer::scan(std::istream& in) noexcept {
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
              add(TokenType::sym_plusplus, "++");
              break;
            }
            case '=': {
              advance_column();
              add(TokenType::sym_plusequal, "+=");
              break;
            }
            case '\0':
            default: {
              add(TokenType::sym_plus, "+");
              break;
            }
          }

          break;
        }

        case '-': {
          switch (peek()) {
            case '-': {
              advance_column();
              add(TokenType::sym_minusminus, "--");
              break;
            }
            case '=': {
              advance_column();
              add(TokenType::sym_minusequal, "-=");
              break;
            }
            case '\0':
            default: {
              add(TokenType::sym_minus, "-");
              break;
            }
          }

          break;
        }

        case '/': {
          compound("//", {TokenType::sym_slash, TokenType::sym_slashslash});
          break;
        }

        case '*': {
          compound("**", {TokenType::sym_star, TokenType::sym_starstar});
          break;
        }

        case '%': {
          add(TokenType::sym_percent, "%");
          break;
        }

        case '=': {
          compound("==", {TokenType::sym_equal, TokenType::sym_equalequal});
          break;
        }

        case '!': {
          compound("==", {TokenType::sym_bang, TokenType::sym_bangequal});
          break;
        }

        case '>': {
          compound(">=", {TokenType::sym_gt, TokenType::sym_gtequal});
          break;
        }

        case '<': {
          compound("<=", {TokenType::sym_lt, TokenType::sym_ltequal});
          break;
        }

        case '&': {
          compound("&&", {TokenType::sym_amp, TokenType::sym_ampamp});
          break;
        }

        case '|': {
          compound("||", {TokenType::sym_pipe, TokenType::sym_pipepipe});
          break;
        }

        case '~': {
          add(TokenType::sym_tilde, "~");
          break;
        }

        case '^': {
          add(TokenType::sym_caret, "^");
          break;
        }

        case ':': {
          add(TokenType::sym_colon, ":");
          break;
        }

        case ';': {
          add(TokenType::sym_semicolon, ";");
          break;
        }

        case '?': {
          add(TokenType::sym_question, "?");
          break;
        }

        case '.': {
          if (std::isdigit(peek())) {
            number(true);
          } else {
            add(TokenType::sym_dot, ".");
          }

          break;
        }

        case ',': {
          add(TokenType::sym_comma, ",");
          break;
        }

        case '#': {
          while (!eol()) {
            advance_column();
          }
          break;
        }

        case '@': {
          add(TokenType::sym_at, "@");
          break;
        }

        case '(': {
          add(TokenType::sym_lround, ")");
          break;
        }
        case ')': {
          add(TokenType::sym_rround, "(");
          break;
        }

        case '[': {
          add(TokenType::sym_lsquare, "[");
          break;
        }
        case ']': {
          add(TokenType::sym_rsquare, "]");
          break;
        }

        case '{': {
          add(TokenType::sym_lbrace, "{");
          break;
        }
        case '}': {
          add(TokenType::sym_rbrace, "}");
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
  if (s.size() != v.size()) return TokenType::identifier;
  for (auto i = 0; i < s.size(); i++) {
    if (s[i] != v[i]) return TokenType::identifier;
  }
  return t;
}

inline TokenType Lexer::keyword(std::string_view sv) {
  switch (sv[0]) {

    case 'f': {
      if (sv.size() < 2) return TokenType::identifier;
      switch (sv[1]) {
        case 'c': return matchesRest("t", sv.substr(2), TokenType::kw_function);
        case 'a':
          return matchesRest("lse", sv.substr(2), TokenType::cnst_false);
        case 'o': return matchesRest("r", sv.substr(2), TokenType::kw_for);
        default: return TokenType::identifier;
      }
    }

    case 'c': {
      if (sv.size() < 2) return TokenType::identifier;
      switch (sv[1]) {
        case 't': return matchesRest("or", sv.substr(2), TokenType::kw_ctor);
        case 'o':
          return matchesRest("ntinue", sv.substr(2), TokenType::kw_continue);
        default: return TokenType::identifier;
      }
    }

    case 'v': {
      if (sv.size() < 3) return TokenType::identifier;
      if (sv[1] != 'i') return TokenType::identifier;
      switch (sv[2]) {
        case 'd': return TokenType::cnst_vid;
        case 'r': return matchesRest("t", sv.substr(3), TokenType::kw_virt);
        default: return TokenType::identifier;
      }
    }

    case 'p': {
      if (sv.size() < 2) return TokenType::identifier;
      if (sv.size() == 2)
        return sv[1] == 'i' ? TokenType::cnst_pi : TokenType::identifier;
      return matchesRest("kg", sv.substr(1), TokenType::kw_package);
    }

    case 't': {
      if (sv.size() < 3) return TokenType::identifier;
      switch (sv[1]) {
        case 'a': return matchesRest("u", sv.substr(2), TokenType::cnst_tau);
        case 'r': return matchesRest("ue", sv.substr(2), TokenType::cnst_true);
      }
    }

    case 'e': {
      if (sv.size() < 2) return TokenType::cnst_eul;
      return matchesRest("lse", sv.substr(1), TokenType::kw_else);
    }

    case 's': return matchesRest("truct", sv.substr(1), TokenType::kw_struct);
    case 'r': return matchesRest("eturn", sv.substr(1), TokenType::kw_break);
    case 'd': return matchesRest("tor", sv.substr(1), TokenType::kw_dtor);
    case 'm': return matchesRest("ain", sv.substr(1), TokenType::kw_main);
    case 'u': return matchesRest("se", sv.substr(1), TokenType::kw_import);
    case 'l': return matchesRest("et", sv.substr(1), TokenType::kw_let);
    case 'i': return matchesRest("f", sv.substr(1), TokenType::kw_if);
    case 'b': return matchesRest("reak", sv.substr(1), TokenType::kw_break);

    default: return TokenType::identifier;
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
    add(TokenType::literal_dbl, num_str);
  } else {
    add(TokenType::literal_int, num_str);
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

  add(TokenType::literal_str, str);
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
