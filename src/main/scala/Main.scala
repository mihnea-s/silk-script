package silkscript

import silkscript.parser.Lexer
import scala.util.parsing.combinator._
import scala.io.Source

object Main extends App {
  for (file <- args) {
    println(s"Parsing $file")

    val input = Source.fromFile(file).getLines().mkString

    Lexer.tokens(input) match {
      case Lexer.Error(msg, _)      => println(s"error: ${msg}")
      case Lexer.Failure(msg, n)    => println(s"failure: ${msg}, next: ${n}")
      case Lexer.Success(result, _) => println(s"success ${result}")
    }
  }
}
