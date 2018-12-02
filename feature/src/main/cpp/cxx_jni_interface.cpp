//
// Created by realxie on 2018/9/4.
//

#include "cxx_jni_interface.h"

#include <stdint.h>
#include <jni.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer
#include <string>

#include "logger.h"
#include "renderer.h"

#define LOG_TAG "EglSample"


static ANativeWindow *window = 0;
static Renderer *renderer = 0;
static std::string sDocumentPath;

JNIEXPORT void JNICALL Java_com_realxie_opengl_demo_feature_MainActivity_nativeSetDocumentPath(JNIEnv* jenv, jobject obj, jstring path)
{
    const char * _path = jenv->GetStringUTFChars(path , NULL ) ;
    sDocumentPath = _path;
}

JNIEXPORT void JNICALL Java_com_realxie_opengl_demo_feature_MainActivity_nativeOnStart(JNIEnv* jenv, jobject obj)
{
    LOG_INFO("nativeOnStart");
    renderer = new Renderer();

    JNIHelper::getInstance();
    return;
}

JNIEXPORT void JNICALL Java_com_realxie_opengl_demo_feature_MainActivity_nativeOnResume(JNIEnv* jenv, jobject obj)
{
    LOG_INFO("nativeOnResume");
    renderer->start();
    return;
}

JNIEXPORT void JNICALL Java_com_realxie_opengl_demo_feature_MainActivity_nativeOnPause(JNIEnv* jenv, jobject obj)
{
    LOG_INFO("nativeOnPause");
    renderer->stop();
    return;
}

JNIEXPORT void JNICALL Java_com_realxie_opengl_demo_feature_MainActivity_nativeOnStop(JNIEnv* jenv, jobject obj)
{
    LOG_INFO("nativeOnStop");
    delete renderer;
    renderer = 0;
    return;
}

JNIEXPORT void JNICALL Java_com_realxie_opengl_demo_feature_MainActivity_nativeSetSurface(JNIEnv* jenv, jobject obj, jobject surface)
{
    if (surface != 0) {
        window = ANativeWindow_fromSurface(jenv, surface);
        LOG_INFO("Got window %p", window);
        renderer->setWindow(window);
    } else {
        LOG_INFO("Releasing window");
        ANativeWindow_release(window);
    }

    return;
}

JNIHelper JNIHelper::m_instance = JNIHelper();

JNIHelper::JNIHelper()
{
//    JavaVM *jvm;
//    JNIEnv *env;
//    JavaVMInitArgs args;
//    JavaVMOption options;
//    args.version = JNI_VERSION_1_6;
//    args.nOptions = 1;
//    options.optionString = "-Djava.class.path=./";
//    args.options = &options;
//    args.ignoreUnrecognized = 0;
//    int rv;
//    rv = JNI_CreateJavaVM(&jvm, &env, &args);
//
//    jclass hello_world_class;
//    jmethodID main_method;
//    jmethodID square_method;
//    jmethodID power_method;
//    jint number=20;
//    jint exponent=3;
//    hello_world_class = env->FindClass("MainActivity");
//    main_method = env->GetStaticMethodID(hello_world_class, "getDocumentPath", ""
//            "()Ljava/lang/String;");
//    env->CallStaticVoidMethod( hello_world_class, main_method, NULL);

}

const std::string& getDocumentPath()
{
    return sDocumentPath;
}

