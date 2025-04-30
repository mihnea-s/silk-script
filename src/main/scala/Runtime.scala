package silkscript

import com.github.sbt.jni.syntax.NativeLoader

class Runtime extends NativeLoader("ssri") {
    @native def init(): Unit

    @native def stepProgram(program: List[Int]): Unit

    @native def setBreakpoint(address: Int): Unit

    @native def setBreakpoint(functionName: String): String
}
