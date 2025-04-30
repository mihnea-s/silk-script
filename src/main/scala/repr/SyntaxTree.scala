package silkscript.repr

import silkscript.parser._

sealed trait SyntaxNode

sealed trait ExpressionNode extends SyntaxNode

case class ValueIdent(v: TokenIdent) extends ExpressionNode
case class ValueNatural(v: TokenNumberWhole) extends ExpressionNode
case class ValueInteger(v: TokenNumberWhole) extends ExpressionNode
case class ValueReal(v: TokenNumberFrac) extends ExpressionNode
case class ValueComplex(r: TokenNumberFrac, i: TokenNumberFrac)
    extends ExpressionNode

case class OperCall(calle: ExpressionNode, args: List[ExpressionNode]) extends ExpressionNode
case class OperUnary(x: ExpressionNode, op: Token) extends ExpressionNode
case class OperDyadic(x: ExpressionNode, op: Token, y: ExpressionNode)
    extends ExpressionNode

sealed trait StatementNode extends SyntaxNode

case class StatementLet(name: String, expr: ExpressionNode) extends StatementNode
case class StatementExpression(v: ExpressionNode) extends StatementNode

sealed trait DeclarationNode extends SyntaxNode

case class DeclFunctionAlias(path: String) extends DeclarationNode
case class DeclTypeAlias(path: String) extends DeclarationNode
case class DeclPackageImport(path: String) extends DeclarationNode
