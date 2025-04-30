package silkscript.parser

import silkscript.repr._

import scala.util.parsing.combinator._
import scala.util.matching.Regex

case object Lexer extends RegexParsers {
  override def skipWhitespace = true
  override val whiteSpace = """[^\S\n]+""".r

  private def ident = """[_\p{L}][_\p{N}\p{L}]*""".r ^^ { TokenIdent(_) }

  private def boolean = {
    "true" ^^^ TokenBoolean(true)
      | "false" ^^^ TokenBoolean(false)
  }

  private def trite = {
    "yes" ^^^ TokenTrite(Trite.Yes)
      | "zero" ^^^ TokenTrite(Trite.Zero)
      | "no" ^^^ TokenTrite(Trite.No)
  }

  private def numberWhole = {
    """0x([a-fA-F\d]+)""".r ^^ { s => TokenNumberWhole(NumberWhole(s)) }
      | """(\d+)""".r ^^ { s => TokenNumberWhole(NumberWhole(s)) }
  }

  private def numberFract = {
    """(\d*[.])?[0-9]+""".r ^^ {
      // TODO
      _ => TokenNumberFrac(NumberFract("", "", ""))
    }
  }

  private def keywords = Array(
    // Meta keywords
    ("let", TokenKeywordLet),
    ("fun", TokenKeywordFun),
    ("type", TokenKeywordType),
    ("use", TokenKeywordUse),

    // Operator keywords
    ("or", TokenOperatorOr),
    ("and", TokenOperatorAnd),
    ("not", TokenOperatorNot),
    ("if", TokenOperatorIf),
    ("else", TokenOperatorElse),
    ("case", TokenOperatorCase),
    ("match", TokenOperatorMatch),
    ("while", TokenOperatorWhile),
    ("for", TokenOperatorFor),
    ("loop", TokenOperatorLoop),

    // Control keywords
    ("break", TokenControlBreak),
    ("continue", TokenControlContinue),
    ("return", TokenControlReturn)
  )
    .map((a, b) => a ^^^ b)
    .reduce((a, b) => a | b)

  private def symbols = Array(
    // Symbols
    ("+", TokenSymPlus),
    ("-", TokenSymMinus),
    ("**", TokenSymStarStar),
    ("*", TokenSymStar),
    ("//", TokenSymSlash),
    ("/", TokenSymSlashSlash),
    ("=", TokenSymEqual),
    (":=", TokenSymColonEqual),
    ("%", TokenSymPercent),
    (",", TokenSymComma),
    (":", TokenSymColon),
    (";", TokenSymSemicolon),
    ("|", TokenSymVertBar),
    ("(", TokenSymRoundL),
    (")", TokenSymRoundR),
    ("[", TokenSymSquareL),
    ("]", TokenSymSquareR),
    ("{", TokenSymBracketL),
    ("}", TokenSymBracketR)
  )
    .map((a, b) => a ^^^ b)
    .reduceLeft((a, b) => a | b)

  private def comments = opt("""#\.*(\n|$)""".r)

  private def literals =
    boolean
      | trite
      | numberFract
      | numberWhole
      | ident

  private def newline = "\n+" ^^^ TokenNewLine

  private def other = """.""".r ^^^ TokenInvalid

  def token = comments ~> (keywords | literals | symbols | other)

  def tokens(in: CharSequence): List[Token] = {
    return parse(phrase(rep(token)), in).get
  }
}
