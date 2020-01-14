#include <istream>
#include <string>
#include <string_view>

#include <silk/ast/token.h>
#include <silk/error.h>
#include <silk/lexer.h>

const std::map<std::string_view, TokenType> Lexer::_keywords {
  {"fct", TokenType::kw_function},    {"struct", TokenType::kw_struct},
  {"ctor", TokenType::kw_ctor},       {"dtor", TokenType::kw_dtor},
  {"super", TokenType::kw_super},     {"virt", TokenType::kw_virt},
  {"this", TokenType::kw_this},

  {"pkg", TokenType::kw_package},     {"main", TokenType::kw_entrypoint},
  {"use", TokenType::kw_import},      {"export", TokenType::kw_export},
  {"with", TokenType::kw_forwarding},

  {"true", TokenType::kw_true},       {"false", TokenType::kw_false},
  {"vid", TokenType::kw_vid},

  {"let", TokenType::kw_let},         {"if", TokenType::kw_if},
  {"else", TokenType::kw_else},       {"for", TokenType::kw_for},
  {"break", TokenType::kw_break},     {"continue", TokenType::kw_continue},
  {"return", TokenType::kw_return},
};

const Lexer::TokensVector& Lexer::scan(std::istream& in) noexcept {
  _tokens.clear();

  try {
    scan_tokens(in);
  } catch (LexingError& e) {
    _error_callback(e);
    return _tokens;
  }

  return _tokens;
}

void Lexer::scan_tokens(std::istream& in) {
  while (std::getline(in, current_line)) {
    advance_line();

    for (; column < current_line.size(); advance_column()) {
      auto c = current_line[column];
      switch (c) {
        case '+': {
          switch (peek()) {
            case '+': {
              advance_column();
              add(TokenType::sym_plusplus);
              break;
            }
            case '=': {
              advance_column();
              add(TokenType::sym_plusequal);
              break;
            }
            case '\0':
            default: {
              add(TokenType::sym_plus);
              break;
            }
          }

          break;
        }

        case '-': {
          switch (peek()) {
            case '-': {
              advance_column();
              add(TokenType::sym_minusminus);
              break;
            }
            case '=': {
              advance_column();
              add(TokenType::sym_minusequal);
              break;
            }
            case '\0':
            default: {
              add(TokenType::sym_minus);
              break;
            }
          }

          break;
        }

        case '/': {
          if (next('/')) {
            advance_column();
            add(TokenType::sym_slashslash);
          } else {
            add(TokenType::sym_slash);
          }

          break;
        }

        case '*': {
          if (next('*')) {
            advance_column();
            add(TokenType::sym_starstar);
          } else {
            add(TokenType::sym_star);
          }

          break;
        }

        case '%': {
          add(TokenType::sym_percent);
          break;
        }

        case '=': {
          if (next('=')) {
            advance_column();
            add(TokenType::sym_equalequal);
          } else {
            add(TokenType::sym_equal);
          }

          break;
        }

        case '!': {
          if (next('=')) {
            advance_column();
            add(TokenType::sym_bangequal);
          } else {
            add(TokenType::sym_bang);
          }
          break;
        }

        case '>': {
          if (next('=')) {
            advance_column();
            add(TokenType::sym_gtequal);
          } else {
            add(TokenType::sym_gt);
          }

          break;
        }

        case '<': {
          if (next('=')) {
            advance_column();
            add(TokenType::sym_ltequal);
          } else {
            add(TokenType::sym_lt);
          }

          break;
        }

        case '&': {
          if (next('&')) {
            advance_column();
            add(TokenType::sym_ampamp);
          } else {
            add(TokenType::sym_amp);
          }
          break;
        }

        case '|': {
          if (next('|')) {
            advance_column();
            add(TokenType::sym_pipepipe);
          } else {
            add(TokenType::sym_pipe);
          }
          break;
        }

        case '~': {
          add(TokenType::sym_tilde);
          break;
        }

        case '^': {
          add(TokenType::sym_caret);
          break;
        }

        case ':': {
          add(TokenType::sym_colon);
          break;
        }

        case ';': {
          add(TokenType::sym_semicolon);
          break;
        }

        case '?': {
          add(TokenType::sym_question);
          break;
        }

        case '.': {
          if (std::isdigit(peek())) {
            number(true);
          } else {
            add(TokenType::sym_dot);
          }

          break;
        }

        case ',': {
          add(TokenType::sym_comma);
          break;
        }

        case '#': {
          while (!eol()) {
            advance_column();
          }
          break;
        }

        case '@': {
          add(TokenType::sym_at);
          break;
        }

        case '(': {
          add(TokenType::sym_lround);
          break;
        }
        case ')': {
          add(TokenType::sym_rround);
          break;
        }

        case '[': {
          add(TokenType::sym_lsquare);
          break;
        }
        case ']': {
          add(TokenType::sym_rsquare);
          break;
        }

        case '{': {
          add(TokenType::sym_lbrace);
          break;
        }
        case '}': {
          add(TokenType::sym_rbrace);
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

          if (_keywords.find(w) != std::end(_keywords)) {
            add(_keywords.at(w));
          } else {
            add(TokenType::identifier, std::string {w});
          }

          break;
        }
      }
    }
  }

  add(TokenType::eof);
}

inline void Lexer::advance_line() {
  line++;
  column = 0;
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
  if (eol()) return '\0';
  return current_line[column + 1];
}

inline bool Lexer::eol() const {
  return column == current_line.size() - 1;
}

inline bool Lexer::alphanum(char c) const {
  return std::isalnum(c) || c == '_';
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
  int start = column;

  while (!eol() && !next('\'')) {
    advance_column();
  }

  advance_column();

  auto str = std::string {
    std::begin(current_line) + start + 1,
    std::begin(current_line) + column,
  };

  add(TokenType::literal_str, str);
}

inline std::string_view Lexer::word() {
  int start = column;

  while (!eol() && alphanum(peek())) {
    advance_column();
  }

  return std::string_view {
    current_line.data() + start,
    static_cast<ulong>(column - start + 1),
  };
};
