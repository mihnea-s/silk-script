package silkscript.repr

enum Trite {
  case Yes, No, Zero
}

sealed case class NumberWhole(decimal: String) {
  def isNegative = decimal.startsWith("-")
  def isPositive = !isNegative
}

sealed case class NumberFract(
    whole: String,
    fraction: String,
    exponent: String
)

case object NumberFract {
  def zero = NumberFract("0", "0", "0")
}
