package silkscript

import silkscript.repr._
import silkscript.parser._
import silkscript.InputSpec

import org.scalacheck.Shrink

class ParserTest extends InputSpec {
  implicit def noShrink[T]: Shrink[T] = Shrink.shrinkAny

  val lexings =
    Table(
      ("tokens", "tree"),
      (
        List(
          TokenNumberWhole(NumberWhole("2")),
          TokenNumberWhole(NumberWhole("3")),
          TokenNewLine,
          TokenNumberWhole(NumberWhole("4"))
        ),
        List(
          ValueNatural(TokenNumberWhole(NumberWhole("2"))),
          ValueNatural(TokenNumberWhole(NumberWhole("3"))),
          ValueNatural(TokenNumberWhole(NumberWhole("4")))
        )
      )
    )

    //   property("the lexer should always produce tokens") {
    //     forAll { (input: List[Token]) =>
    //       noException should be thrownBy (Parser.tree(input))
    //     }
    //   }

  property("the lexer should produce correct tokens") {
    forAll(lexings) { (input, tree) =>
      Parser.tree(input) should equal(tree)
    }
  }
}
