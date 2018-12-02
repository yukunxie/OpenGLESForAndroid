//
// Created by realxie on 2018/9/4.
//

#ifndef ANDROIDOPENGLES_CXX_JNI_INTERFACE_H
#define ANDROIDOPENGLES_CXX_JNI_INTERFACE_H

#include <jni.h>
#include <string>

extern "C" {
JNIEXPORT void JNICALL Java_com_realxie_opengl_demo_feature_MainActivity_getString(JNIEnv
                                                                                       *env, jobject /* this */);


};


extern "C" {
JNIEXPORT void JNICALL Java_com_realxie_opengl_demo_feature_MainActivity_nativeSetDocumentPath
        (JNIEnv* jenv, jobject obj, jstring path);
JNIEXPORT void JNICALL Java_com_realxie_opengl_demo_feature_MainActivity_nativeOnStart(JNIEnv* jenv, jobject obj);
JNIEXPORT void JNICALL Java_com_realxie_opengl_demo_feature_MainActivity_nativeOnResume(JNIEnv* jenv, jobject obj);
JNIEXPORT void JNICALL Java_com_realxie_opengl_demo_feature_MainActivity_nativeOnPause(JNIEnv* jenv, jobject obj);
JNIEXPORT void JNICALL Java_com_realxie_opengl_demo_feature_MainActivity_nativeOnStop(JNIEnv* jenv, jobject obj);
JNIEXPORT void JNICALL Java_com_realxie_opengl_demo_feature_MainActivity_nativeSetSurface(JNIEnv* jenv, jobject obj, jobject surface);
};


class JNIHelper
{
public:
    static JNIHelper& getInstance()
    {
        return m_instance;
    }

private:
    JNIHelper();

private:
    static JNIHelper m_instance;
};

const std::string& getDocumentPath();

#endif //ANDROIDOPENGLES_CXX_JNI_INTERFACE_H
