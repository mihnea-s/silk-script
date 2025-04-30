#include <silkscript_Emitter.h>

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "mem.h"
#include "opcode.h"
#include "program.h"

#define JNI_SIG_LONG "J"
#define JNI_SIG_BYTE_BUFFER "Ljava/nio/ByteBuffer;"
#define JNI_SIG_BYTE_BUFFER_POS "Ljava/nio/ByteBuffer;"

#define SILK_INIT_BYTECOUNT 1 << 8
#define SILK_GROW_BYTEMULTI 2.0

static void emitter_emit_byte(JNIEnv *jenv, jobject jobj, uint8_t opcode) {
  jclass jcls = (*jenv)->GetObjectClass(jenv, jobj);

  jfieldID fid = (*jenv)->GetFieldID(jenv, jcls, "ptr", JNI_SIG_LONG);
  assert(((void)"ptr field id is NULL", fid != NULL));

  Program *ptr = (Program *)(*jenv)->GetLongField(jenv, jobj, fid);
  write_byte(ptr, opcode);
}

JNIEXPORT void JNICALL Java_silkscript_Emitter_init(JNIEnv *jenv,
                                                    jobject jobj) {
  jclass jcls = (*jenv)->GetObjectClass(jenv, jobj);

  jfieldID fid = (*jenv)->GetFieldID(jenv, jcls, "ptr", JNI_SIG_LONG);
  assert(((void)"ptr field id is NULL", fid != NULL));

  Program *ptr = memory(NULL, 0, sizeof(Program));
  init_program(ptr, SILK_INIT_BYTECOUNT, SILK_INIT_BYTECOUNT,
               SILK_INIT_BYTECOUNT);

  (*jenv)->SetLongField(jenv, jobj, fid, (long)ptr);
}

JNIEXPORT void JNICALL Java_silkscript_Emitter_emitFin(JNIEnv *jenv,
                                                       jobject jobj) {
  emitter_emit_byte(jenv, jobj, VM_FIN);
}

JNIEXPORT void JNICALL Java_silkscript_Emitter_emitNop(JNIEnv *jenv,
                                                       jobject jobj) {
  emitter_emit_byte(jenv, jobj, VM_NOP);
}

JNIEXPORT void JNICALL Java_silkscript_Emitter_emitGC(JNIEnv *jenv,
                                                      jobject jobj) {
  emitter_emit_byte(jenv, jobj, VM_GC);
}

JNIEXPORT void JNICALL Java_silkscript_Emitter_emitDbg(JNIEnv *jenv,
                                                       jobject jobj) {
  emitter_emit_byte(jenv, jobj, VM_DBG);
}
