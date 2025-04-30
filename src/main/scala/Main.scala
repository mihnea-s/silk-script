package silkscript

import silkscript.repr._
import silkscript.parser.Lexer
import silkscript.parser.Parser
import scala.util.parsing.combinator._
import scala.io.{Source, StdIn}
import java.nio.ByteBuffer

def printTree(node: SyntaxNode): String = node match
  case ValueIdent(v)        => s"${v}"
  case ValueNatural(v)      => s"${v}"
  case ValueInteger(v)      => s"${v}"
  case ValueReal(v)         => s"${v}"
  case ValueComplex(r, i)   => s"${r} + ${i}i"
  case OperUnary(x, op)     => s"(${op} ${printTree(x)})"
  case OperDyadic(x, op, y) => s"(${op} ${printTree(x)} ${printTree(y)})"
  case OperCall(calle, args) =>
    s"${printTree(calle)}(${args.map(printTree).mkString(", ")})"
  case StatementExpression(v) => s"${printTree(v)};"
  case _                      => "<<other>>"

def parseStdinLine: Nothing =
  val line = StdIn.readLine()
  Parser.tree(Lexer.tokens(line)) map printTree foreach println
  parseStdinLine

object Main extends App {
  val emitter = Emitter()
  emitter.init()
  emitter.emitGC()
  emitter.emitGC()
  emitter.emitNop()
  emitter.emitFin()
  emitter.emitDbg()
  emitter.emitGC()

  parseStdinLine
  // for (file <- args) {
  //   println(s"Parsing $file")

  //   val input = Source.fromFile(file).getLines().mkString

  //   println(Lexer.tokens(input))
  // }

  val input = "2 + 3i * 4 + 5i"

  val tokens = Lexer.tokens(input)
  println(s"Tokens = ${tokens}\n\n")

  val syntaxTree = Parser.tree(tokens)
  syntaxTree.map(printTree).foreach(println)
  // println(s"Syntax Tree = ${syntaxTree}\n\n")
}
