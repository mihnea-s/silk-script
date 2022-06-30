package silkscript.parser

sealed abstract class Token

case class Ident(str: String) extends Token
case class Number(num: Float) extends Token
case class Bool(bol: Boolean) extends Token

case object KwLet extends Token
case object KwDef extends Token

case object KwOr extends Token
case object KwAnd extends Token
case object KwNot extends Token

case object KwIf extends Token
case object KwElse extends Token
case object KwSwitch extends Token
case object KwMatch extends Token

case object KwWhile extends Token
case object KwBreak extends Token
case object KwContinue extends Token

case object KwLoop extends Token
case object KwFor extends Token

case object KwFun extends Token
case object KwReturn extends Token

case object KwEnum extends Token
case object KwStruct extends Token

case object KwUse extends Token

case object SymPlus extends Token
