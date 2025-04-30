package silkscript

import com.github.sbt.jni.syntax.NativeLoader
import java.nio.ByteBuffer

class Emitter extends NativeLoader("ssri") {
  var bytecode: ByteBuffer = null

  val roDataPtr: Long = 0
  val symTablePtr: Long = 0

  @native def init(): Unit

  @native def emitFin(): Unit
  @native def emitNop(): Unit
  @native def emitGC(): Unit
  @native def emitDbg(): Unit
}
