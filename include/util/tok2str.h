#pragma once

#include <fmt/core.h>

#include <silk/ast/token.h>

// clang-format off

std::string tok2str(Token tok) {
  switch (tok.type()) {
    case TokenType::identifier: return fmt::format("@{}", tok.lexeme());
    case TokenType::literal_dbl: return fmt::format("{}f", tok.lexeme());
    case TokenType::literal_int: return fmt::format("{}i64", tok.lexeme());
    case TokenType::literal_str: return fmt::format("`{}`", tok.lexeme());

    case TokenType::sym_plus: return "+";
    case TokenType::sym_minus: return "-";
    case TokenType::sym_slash: return "/";
    case TokenType::sym_star: return "*";
    case TokenType::sym_slashslash: return "//";
    case TokenType::sym_starstar: return "**";
    case TokenType::sym_percent: return "%";

    case TokenType::sym_equalequal: return "==";
    case TokenType::sym_bangequal: return "!=";
    case TokenType::sym_gt: return ">";
    case TokenType::sym_gtequal: return ">=";
    case TokenType::sym_lt: return "<";
    case TokenType::sym_ltequal: return "<=";

    case TokenType::sym_pipepipe: return "||";
    case TokenType::sym_ampamp: return "&&";

    case TokenType::sym_amp: return "&";
    case TokenType::sym_pipe: return "|";
    case TokenType::sym_caret: return "^";

    case TokenType::sym_tilde: return "~";
    case TokenType::sym_bang: return "!";

    case TokenType::kw_function: return "fct";
    case TokenType::kw_struct: return "struct";

    case TokenType::kw_import: return "use";
    case TokenType::kw_export: return "export";
    case TokenType::kw_forwarding: return "with";
    case TokenType::kw_entrypoint: return "entrypoint";

    case TokenType::kw_virt: return "virt";
    case TokenType::kw_this: return "this";
    case TokenType::kw_ctor: return "ctor";
    case TokenType::kw_dtor: return "dtor";

    case TokenType::kw_let: return "let";
    case TokenType::kw_if: return "if";
    case TokenType::kw_else: return "else";
    case TokenType::kw_for: return "for";
    case TokenType::kw_return: return "return";

    case TokenType::sym_lround: return "(";
    case TokenType::sym_rround: return ")";
    
    case TokenType::sym_lsquare: return "[";
    case TokenType::sym_rsquare: return "]";

    case TokenType::sym_lbrace: return "{";
    case TokenType::sym_rbrace: return "}";

    case TokenType::sym_plusplus: return "++";
    case TokenType::sym_minusminus: return "--";
    case TokenType::sym_semicolon: return ";";

    case TokenType::sym_equal: return "=";

    case TokenType::eof: return "eof";

    default: {
      return "??";
    }
  }
}