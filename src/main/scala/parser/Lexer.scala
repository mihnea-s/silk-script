package silkscript.parser

import scala.util.parsing.combinator._
import scala.util.matching.Regex

object Lexer extends RegexParsers {

  def ident = """[_\p{L}][_\p{N}\p{L}]*""".r ^^ { Ident(_) }
  def number = """-?\d+(\.\d+)?""".r ^^ { _.toFloat } ^^ { Number(_) }

  def symPlus = "+" ^^^ SymPlus

  def kwLet = "let" ^^ (_ => KwLet)
  def kwDef = "def" ^^ (_ => KwDef)

  def kwOr = "or" ^^ (_ => KwOr)
  def kwAnd = "and" ^^ (_ => KwAnd)
  def kwNot = "not" ^^ (_ => KwNot)

  def tokens(in: CharSequence): ParseResult[List[Token]] = {

    val literals = ident | number

    val symbols = symPlus

    val keywords = kwLet | kwDef | kwOr | kwAnd | kwNot

    return parse(phrase(rep1(symbols | keywords | literals)), in)
  }
}
