#include <silk/language/package.h>

#include <optional>

namespace silk {

auto read_package(std::filesystem::path root_path) -> std::optional<Package> {
  // TODO: implement
  return std::nullopt;
}

auto token_kind_string(TokenKind kind) -> std::string_view {
  switch (kind) {
    case TokenKind::IDENTIFIER: return "identifier";
    case TokenKind::TOK_END: return "end of file";
    case TokenKind::TOK_ERROR: return "error";
    case TokenKind::KW_MAIN: return "keyword `main`";
    case TokenKind::KW_PKG: return "keyword `pkg`";
    case TokenKind::KW_USE: return "keyword `use`";
    case TokenKind::KW_FUN: return "keyword `fun`";
    case TokenKind::KW_ENUM: return "keyword `enum`";
    case TokenKind::KW_OBJ: return "keyword `obj`";
    case TokenKind::KW_PRIV: return "keyword `priv`";
    case TokenKind::KW_DLL: return "keyword `dll`";
    case TokenKind::KW_MACRO: return "keyword `macro`";
    case TokenKind::KW_LET: return "keyword `let`";
    case TokenKind::KW_DEF: return "keyword `def`";
    case TokenKind::KW_CONST: return "keyword `const`";
    case TokenKind::KW_RETURN: return "keyword `return`";
    case TokenKind::KW_SWITCH: return "keyword `switch`";
    case TokenKind::KW_BREAK: return "keyword `break`";
    case TokenKind::KW_CONTINUE: return "keyword `continue`";
    case TokenKind::KW_IF: return "keyword `if`";
    case TokenKind::KW_ELSE: return "keyword `else`";
    case TokenKind::KW_WHILE: return "keyword `while`";
    case TokenKind::KW_LOOP: return "keyword `loop`";
    case TokenKind::KW_FOR: return "keyword `for`";
    case TokenKind::KW_FOREACH: return "keyword `foreach`";
    case TokenKind::KW_MATCH: return "keyword `match`";
    case TokenKind::KW_AND: return "keyword `and`";
    case TokenKind::KW_OR: return "keyword `or`";
    case TokenKind::KW_NOT: return "keyword `not`";
    case TokenKind::BOOL_TRUE: return "keyword `true`";
    case TokenKind::BOOL_FALSE: return "keyword `false`";
    case TokenKind::KEY_VOID: return "keyword `void`";
    case TokenKind::KEY_PI: return "keyword `pi`";
    case TokenKind::KEY_TAU: return "keyword `tau`";
    case TokenKind::KEY_EUL: return "keyword `e`";
    case TokenKind::LITERAL_NAT: return "natural";
    case TokenKind::LITERAL_INT: return "integer";
    case TokenKind::LITERAL_REAL: return "real";
    case TokenKind::LITERAL_CHAR: return "character";
    case TokenKind::LITERAL_STRING: return "string";
    case TokenKind::SYM_COMMA: return "`,`";
    case TokenKind::SYM_SEMICOLON: return "`;`";
    case TokenKind::SYM_RD_OPEN: return "`(`";
    case TokenKind::SYM_RD_CLOSE: return "`)`";
    case TokenKind::SYM_SQ_OPEN: return "`[`";
    case TokenKind::SYM_SQ_CLOSE: return "`]`";
    case TokenKind::SYM_BR_OPEN: return "`{`";
    case TokenKind::SYM_BR_CLOSE: return "`}`";
    case TokenKind::SYM_PLUS: return "`+`";
    case TokenKind::SYM_PLUS_PLUS: return "`++`";
    case TokenKind::SYM_PLUS_EQUAL: return "`+=`";
    case TokenKind::SYM_PLUS_PLUS_EQUAL: return "`++=`";
    case TokenKind::SYM_MINUS: return "`-`";
    case TokenKind::SYM_ARROW: return "`->`";
    case TokenKind::SYM_MINUS_MINUS: return "`--`";
    case TokenKind::SYM_MINUS_EQUAL: return "`-=`";
    case TokenKind::SYM_MINUS_MINUS_EQUAL: return "`--=`";
    case TokenKind::SYM_SLASH: return "`/`";
    case TokenKind::SYM_SLASH_SLASH: return "`//`";
    case TokenKind::SYM_SLASH_EQUAL: return "`/=`";
    case TokenKind::SYM_SLASH_SLASH_EQUAL: return "`//=`";
    case TokenKind::SYM_STAR: return "`*`";
    case TokenKind::SYM_STAR_STAR: return "`**`";
    case TokenKind::SYM_STAR_EQUAL: return "`*=`";
    case TokenKind::SYM_STAR_STAR_EQUAL: return "`**=`";
    case TokenKind::SYM_PERC: return "`%`";
    case TokenKind::SYM_PERC_PERC: return "`%%`";
    case TokenKind::SYM_PERC_EQUAL: return "`%=`";
    case TokenKind::SYM_PERC_PERC_EQUAL: return "`%%=`";
    case TokenKind::SYM_EQUAL: return "`=`";
    case TokenKind::SYM_EQUAL_EQUAL: return "`==`";
    case TokenKind::SYM_FATARROW: return "`=>`";
    case TokenKind::SYM_EQUAL_EQUAL_EQUAL: return "`===`";
    case TokenKind::SYM_BANG: return "`!`";
    case TokenKind::SYM_BANG_BANG: return "`!!`";
    case TokenKind::SYM_BANG_EQUAL: return "`!=`";
    case TokenKind::SYM_BANG_BANG_EQUAL: return "`!!=`";
    case TokenKind::SYM_GT: return "`>`";
    case TokenKind::SYM_GT_GT: return "`>>`";
    case TokenKind::SYM_GT_GT_GT: return "`>>>`";
    case TokenKind::SYM_GT_EQUAL: return "`>=`";
    case TokenKind::SYM_GT_GT_EQUAL: return "`>>=`";
    case TokenKind::SYM_GT_GT_GT_EQUAL: return "`>>>=`";
    case TokenKind::SYM_LT: return "`<`";
    case TokenKind::SYM_LT_ARROW: return "`<-`";
    case TokenKind::SYM_LT_LT: return "`<<`";
    case TokenKind::SYM_LT_LT_LT: return "`<<<`";
    case TokenKind::SYM_LT_EQUAL: return "`<=`";
    case TokenKind::SYM_LT_LT_EQUAL: return "`<<=`";
    case TokenKind::SYM_LT_LT_LT_EQUAL: return "`<<<=`";
    case TokenKind::SYM_AMP: return "`&`";
    case TokenKind::SYM_AMP_AMP: return "`&&`";
    case TokenKind::SYM_PIPE: return "`|`";
    case TokenKind::SYM_PIPE_PIPE: return "`||`";
    case TokenKind::SYM_TILDE: return "`~`";
    case TokenKind::SYM_TILDE_TILDE: return "`~~`";
    case TokenKind::SYM_CARET: return "`^`";
    case TokenKind::SYM_CARET_CARET: return "`^^`";
    case TokenKind::SYM_QMARK: return "`?`";
    case TokenKind::SYM_QMARK_QMARK: return "`??`";
    case TokenKind::SYM_ATSIGN: return "`@`";
    case TokenKind::SYM_ATSIGN_ATSIGN: return "`@@`";
    case TokenKind::SYM_USCORE: return "``_";
    case TokenKind::SYM_USCORE_USCORE: return "``__";
    case TokenKind::SYM_DOT: return "`.`";
    case TokenKind::SYM_DOT_DOT: return "`..`";
    case TokenKind::SYM_DOT_DOT_DOT: return "`...`";
    case TokenKind::SYM_DOT_BRACE: return "`.{`";
    case TokenKind::SYM_COLON: return "`:`";
    case TokenKind::SYM_COLON_COLON: return "`::`";
    case TokenKind::SYM_COLON_BRACE: return "`:{`";
    case TokenKind::SYM_DOLLAR: return "`$`";
    case TokenKind::SYM_DOLLAR_DOLLAR: return "`$$`";
    case TokenKind::SYM_DOLLAR_BRACE: return "`${`";
    case TokenKind::SYM_HASH_BRACE: return "`#{`";
    default: return "<unkown token>";
  }
}

} // namespace silk