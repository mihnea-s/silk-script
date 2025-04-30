package silkscript

import org.scalatest._
import org.scalatest.flatspec._
import org.scalatest.propspec._
import org.scalatest.matchers.should._
import org.scalatestplus.scalacheck._

abstract class UnitSpec
    extends AnyFlatSpec
    with Matchers
    with OptionValues
    with Inside
    with Inspectors

abstract class InputSpec
    extends AnyPropSpec
    with ScalaCheckPropertyChecks
    with Matchers
    with OptionValues
    with Inside
    with Inspectors
