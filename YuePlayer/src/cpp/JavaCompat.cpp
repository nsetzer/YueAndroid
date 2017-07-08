#include "JavaCompat.h"
#include "MediaCtrlRemoteServer.h"

static void JNIServicePrevCallback(JNIEnv *env, jobject obj)
{
    Q_UNUSED(env);
    Q_UNUSED(obj);
    qDebug() << "jni native call: prev";
    MediaCtrlRemoteServer::instance()->prev();
}

static void JNIServiceNextCallback(JNIEnv *env, jobject obj)
{
    Q_UNUSED(env);
    Q_UNUSED(obj);
    qDebug() << "jni native call: next";
    MediaCtrlRemoteServer::instance()->next();
}

static void JNIServicePlayPauseCallback(JNIEnv *env, jobject obj)
{
    Q_UNUSED(env);
    Q_UNUSED(obj);
    qDebug() << "jni native call: playpause";
    MediaCtrlRemoteServer::instance()->playpause();
}

static JNINativeMethod ServiceArray[] =
{
    {"JNIServicePrevCallback", "()V", (void*)JNIServicePrevCallback},
    {"JNIServiceNextCallback", "()V", (void*)JNIServiceNextCallback},
    {"JNIServicePlayPauseCallback", "()V", (void*)JNIServicePlayPauseCallback},
};

int JavaCompat::RegisterNatives() {
#ifdef Q_OS_ANDROID
    QAndroidJniEnvironment env;
    jclass javaClass;

    javaClass = env->FindClass("org/github/nsetzer/example/MyCustomAppService");
    if(!javaClass)
        return JNI_ERR;
    if(env->RegisterNatives(javaClass, ServiceArray,
       sizeof(ServiceArray) / sizeof(ServiceArray[0])) < 0)
        return JNI_ERR;

    return JNI_VERSION_1_6;
#else
    return -1;
#endif
}
