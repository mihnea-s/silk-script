package silkscript.parser

import silkscript.repr._

import scala.util.parsing.input._
import scala.util.parsing.combinator._
import java.text.ParseException

case object Parser extends Parsers {
  override type Elem = Token

  private val infixOperators = collection.immutable.HashMap[Token, (Int, Int)](
    TokenSymPlus -> Prec.One.RAssoc,
    TokenSymMinus -> Prec.One.RAssoc,
    TokenSymStar -> Prec.Two.LAssoc,
    TokenSymSlash -> Prec.Two.RAssoc
  )

  sealed abstract class Prec(base: Int) {
    val RAssoc = (base, base + 1)
    val LAssoc = (base + 1, base)
  }

  case object Prec {
    case object One extends Prec(1)
    case object Two extends Prec(3)
    case object Three extends Prec(5)
  }

  private def ident = accept(
    "identifier",
    { case tok @ TokenIdent(_) =>
      tok
    }
  )

  private def numberLiteral = accept(
    "number literal",
    {
      case tok @ TokenNumberWhole(v) if v.isPositive => ValueNatural(tok)
      case tok @ TokenNumberWhole(v) if v.isNegative => ValueInteger(tok)
      case tok @ TokenNumberFrac(v)                  => ValueReal(tok)
    }
  )

  private def numberReal: Parser[TokenNumberFrac] = accept(
    "real number",
    { case tok @ TokenNumberFrac(_) =>
      tok
    }
  )

  private def numberComplex = {
    numberReal
      ~ (TokenSymPlus | TokenSymMinus)
      ~ numberReal
      <~ TokenIdent("i")
      ^^ { case r ~ op ~ i =>
        ValueComplex(r, i)
      }
  }

  private def number = numberComplex | numberLiteral

  private def unaryPrefixExpression = number | (ident ^^ { ValueIdent(_) })

  private def callExpression(value: ExpressionNode): Parser[ExpressionNode] =
    TokenSymRoundL ~> repsep(expression, TokenSymComma) <~ TokenSymRoundR ^^ {
      case args => OperCall(value, args)
    }

  private def unaryPostfixExpression(value: ExpressionNode) =
    callExpression(value) | success(value)

  private def dyadicExpression = new Parser[ExpressionNode] {
    override def apply(in: Input): ParseResult[ExpressionNode] =
      reduceRight(0)(in)

    private def acceptHigherPrecedence(minPrec: Int): Parser[(Token, Int)] =
      accept(
        "dyadic operator",
        { (tok: Token) =>
          infixOperators.get(tok).flatMap {
            case (lpr, rpr) if lpr >= minPrec => Some(tok, rpr)
            case _                            => None
          }
        }.unlift
      )

    private def reduceRight(minLeftFactor: Int): Parser[ExpressionNode] =
      (unaryPrefixExpression ~ acceptHigherPrecedence(
        minLeftFactor
      ).?) flatMap {
        case left ~ None => unaryPostfixExpression(left)
        case left ~ Some(op, rightFactor) =>
          reduceRight(rightFactor) ^^ { right => OperDyadic(left, op, right) }
      }
  }

  private def expression = dyadicExpression

  private def statementLet: Parser[StatementNode] =
    (TokenKeywordLet ~> ident <~ TokenSymEqual) ~ expression ^^ {
      case TokenIdent(name) ~ expr => StatementLet(name, expr)
    }

  private def statement: Parser[StatementNode] = statementLet | (expression ^^ {
    StatementExpression(_)
  })

  class TokenReader(tokens: Seq[Token]) extends Reader[Token] {
    override def first: Token = tokens.head
    override def atEnd: Boolean = tokens.isEmpty
    override def pos: Position = NoPosition
    override def rest: Reader[Token] = new TokenReader(tokens.tail)
  }

  def tree(in: Seq[Token]): List[SyntaxNode] = {
    return phrase(repsep(statement, TokenNewLine.*))
      .apply(TokenReader(in)) match
      case Success(result, _) => result
      case Failure(msg, next) => throw ParseException(msg, 0)
      case Error(msg, next)   => throw ParseException(msg, 0)
  }
}
