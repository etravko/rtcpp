#include <jni.h>
#include <stdio.h>

#include "rtcpp.h"
#include <fstream>
#include <sstream>

// automatically generated
#include "org_km_rtcppjni_RTPTools.h"

jint throwJavaException( JNIEnv *env, const char *msg )
{
    std::string className = "java/lang/Exception";
    jclass exClass = env->FindClass(className.c_str());
    return env->ThrowNew(exClass, msg);
}

jstring Java_org_km_rtcppjni_RTPTools_AnalyzeRTPDump
  (JNIEnv *env, jclass, jobject buffer)
{
    if (!buffer) {
        throwJavaException(env, "jni: input buffer is null");
    }

    std::string output;

    void* dmaBuffer = env->GetDirectBufferAddress(buffer);
    size_t len = env->GetDirectBufferCapacity(buffer);

    try {
        rtcpp::MemoryDataSource ds((uint8_t*)dmaBuffer, len);
        output = rtcpp::AnalyzeRTPDump(ds);
    }
    catch (std::exception& e) {
        throwJavaException(env, e.what());
    }

    return env->NewStringUTF(output.c_str());
}

void Java_org_km_rtcppjni_RTPTools_TransformRTPDump
        (JNIEnv *env, jclass, jobject inBuf, jobject outBuf, jstring jsonStr)
{
    if (!inBuf || !outBuf || !jsonStr) {
        throwJavaException(env, "jni: input or output buffers, or json are null");
    }

    void* dmaInBuf = env->GetDirectBufferAddress(inBuf);
    size_t dmaInBufLen = env->GetDirectBufferCapacity(inBuf);

    void* dmaOutBuf = env->GetDirectBufferAddress(outBuf);
    size_t dmaOutBufLen = env->GetDirectBufferCapacity(outBuf);

    try {
        rtcpp::MemoryDataSource inDs((uint8_t*)dmaInBuf, dmaInBufLen);
        rtcpp::MemoryDataSink outDs((uint8_t*)dmaOutBuf, dmaOutBufLen);

        jboolean isCopy;
        const char *cJson = env->GetStringUTFChars(jsonStr, &isCopy);
        std::string jsonCpp = std::string(cJson);
        env->ReleaseStringUTFChars(jsonStr, cJson);

        rtcpp::TransformRTPDump(inDs, outDs, jsonCpp);

    } catch (std::exception& e) {
        throwJavaException(env, e.what());
    }
}