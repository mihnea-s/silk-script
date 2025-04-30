#include <silkscript_Runtime.h>

JNIEXPORT void JNICALL Java_silkscript_Runtime_init(JNIEnv *jenv,
                                                    jobject jobj) {
  return;
}

JNIEXPORT void JNICALL Java_silkscript_Runtime_stepProgram(JNIEnv *jenv,
                                                           jobject jobj,
                                                           jobject progList) {
  return;
}

JNIEXPORT void JNICALL Java_silkscript_Runtime_setBreakpoint__I(JNIEnv *jenv,
                                                                jobject jobj,
                                                                jint addr) {
  return;
}

JNIEXPORT jstring JNICALL
Java_silkscript_Runtime_setBreakpoint__Ljava_lang_String_2(JNIEnv *jenv,
                                                           jobject jobj,
                                                           jstring fn_name) {
  if ((*jenv)->ExceptionCheck(jenv)) {
    return (*jenv)->NewStringUTF(jenv, "Exception ocurred!");
  }

  return (*jenv)->NewStringUTF(jenv, "No exception");
}
