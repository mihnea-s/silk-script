package silkscript

import silkscript.parser._
import silkscript.repr._

import org.scalacheck.Shrink

class LexerTest extends InputSpec {
  implicit def noShrink[T]: Shrink[T] = Shrink.shrinkAny

  val lexings =
    Table(
      ("input", "tokens"),
      ("", Nil),
      ("#", Nil),
      ("#\n#", TokenNewLine :: Nil),
      ("\n  \t \n\n", TokenNewLine :: Nil),
      ("[]", TokenSymSquareL :: TokenSymSquareR :: Nil),
      (
        "abc01_bc 01",
        TokenIdent("abc01_bc") :: TokenNumberWhole(NumberWhole("01")) :: Nil
      ),
      ("㰝", TokenIdent("㰝") :: Nil)
    )

  property("the lexer should always produce tokens") {
    forAll { (input: String) =>
      noException should be thrownBy (Lexer.tokens(input))
    }
  }

  property("the lexer should produce correct tokens") {
    forAll(lexings) { (input, tokens) =>
      Lexer.tokens(input) should equal(tokens)
    }
  }
}
