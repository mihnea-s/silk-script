package silkscript.parser

import silkscript.repr._

sealed abstract class Token

case object TokenInvalid extends Token
case object TokenNewLine extends Token
case class TokenIdent(name: String) extends Token

case class TokenBoolean(value: Boolean) extends Token
case class TokenTrite(value: Trite) extends Token

case class TokenNumberWhole(value: NumberWhole) extends Token
case class TokenNumberFrac(value: NumberFract) extends Token

case object TokenKeywordLet extends Token
case object TokenKeywordFun extends Token
case object TokenKeywordType extends Token
case object TokenKeywordUse extends Token

case object TokenOperatorOr extends Token
case object TokenOperatorAnd extends Token
case object TokenOperatorNot extends Token

case object TokenOperatorIf extends Token
case object TokenOperatorElse extends Token
case object TokenOperatorCase extends Token
case object TokenOperatorMatch extends Token

case object TokenOperatorWhile extends Token
case object TokenOperatorFor extends Token
case object TokenOperatorLoop extends Token

case object TokenControlBreak extends Token
case object TokenControlContinue extends Token
case object TokenControlReturn extends Token

case object TokenSymPlus extends Token
case object TokenSymMinus extends Token
case object TokenSymStar extends Token
case object TokenSymStarStar extends Token
case object TokenSymSlash extends Token
case object TokenSymSlashSlash extends Token

case object TokenSymEqual extends Token
case object TokenSymColonEqual extends Token

case object TokenSymPercent extends Token
case object TokenSymComma extends Token
case object TokenSymColon extends Token
case object TokenSymSemicolon extends Token
case object TokenSymVertBar extends Token
case object TokenSymRoundL extends Token
case object TokenSymRoundR extends Token
case object TokenSymSquareL extends Token
case object TokenSymSquareR extends Token
case object TokenSymBracketL extends Token
case object TokenSymBracketR extends Token
