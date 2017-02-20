#include <jni.h>
#include <UDPStreamerJNI.h>
#include <udp_streaming_lib.h>
/* native impl */

JNIEXPORT void JNICALL Java_UDPStreamerJNI_native_1blocking_1receive
(JNIEnv *, jobject) {
    native_blocking_receive();
}

JNIEXPORT void JNICALL Java_UDPStreamerJNI_native_1send
(JNIEnv *, jobject) {
    native_send();
}

