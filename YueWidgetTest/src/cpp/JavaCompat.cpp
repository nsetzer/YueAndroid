#include "JavaCompat.h"
#include "MediaCtrlRemoteServer.h"
#include <QCoreApplication>

#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
#include "yue/bell/MediaCtrlBase.h"

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#endif

#ifdef Q_OS_ANDROID

static void JNIActivitySTTResultCallback(JNIEnv *env, jobject obj, jstring jstra, jstring jstrb)
{
    Q_UNUSED(obj);
    const char * result1= env->GetStringUTFChars(jstra , nullptr);
    const char * result2= env->GetStringUTFChars(jstrb , nullptr);

    QStringList results;
    results.push_back(result1);
    results.push_back(result2);
    for (QString result : results) {
        QStringList words = result.split(" ");
        if ( words.length() > 1 && words[0].toLower() == "play") {
            words.removeAt(0);
            QString query = words.join(" ");
            qDebug() << "jni native call: play song: " << query;
            QList<yue::bell::Database::uid_t> lst = yue::bell::Library::instance()->createPlaylist(query,1);
            if (lst.length()>0) {
                yue::bell::MediaCtrlBase::instance()->doPlaySong(lst[0]);
                return;
            }
        } else if ( words.length() > 1 && words[0].toLower() == "create") {
            words.removeAt(0);
            if (words.length() > 1 && (words[0].toLower() == "playlist" || words[0].toLower() == "playlists")) {
                words.removeAt(0);
            }
            if (words.length() > 1 && words[0].toLower() == "with") {
                words.removeAt(0);
            }

            QString query = words.join(" ");
            qDebug() << "jni native call: create: " << query;
            QList<yue::bell::Database::uid_t> lst = yue::bell::Library::instance()->createPlaylist(query);
            if (lst.length()>0) {
                yue::bell::MediaCtrlBase::instance()->doSetCurrentPlaylist(lst,true);
                return;
            }
        } else {
            qDebug() << "jni native call: default: " << result;
        }
    }

    qDebug() << "jni native call: unable to recognise input";

    QAndroidJniObject message = QAndroidJniObject::fromString("Unrecognized Input: "+ results[0]);
    QAndroidJniObject::callStaticMethod<void>("org/github/nsetzer/example/MyCustomAppActivity",
                                       "makeToast",
                                       "(Ljava/lang/String;)V",
                                       message.object<jstring>());

}

static JNINativeMethod ActivityArray[] =
{
    {"JNIActivitySTTResultCallback", "(Ljava/lang/String;Ljava/lang/String;)V", (void*)JNIActivitySTTResultCallback},
};

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
    qDebug() << "jni native call: exit app";
    MediaCtrlRemoteServer::instance()->playpause();
}

static void JNIServiceExtAppCallback(JNIEnv *env, jobject obj)
{
    Q_UNUSED(env);
    Q_UNUSED(obj);
    qDebug() << "jni native call: exit app";
    try {
        // stop all threads when the service is destroyed
        // otherwise the service can stay alive, and continue playing music.
        MediaCtrlRemoteServer::instance()->stop();

        QCoreApplication::exit(0);
    } catch(...) {
        qDebug() << "jni native call: exit app - error";
    }
}

static JNINativeMethod ServiceArray[] =
{
    {"JNIServicePrevCallback", "()V", (void*)JNIServicePrevCallback},
    {"JNIServiceNextCallback", "()V", (void*)JNIServiceNextCallback},
    {"JNIServicePlayPauseCallback", "()V", (void*)JNIServicePlayPauseCallback},
    {"JNIServiceExtAppCallback", "()V", (void*)JNIServiceExtAppCallback},
};

#endif

int JavaCompat::RegisterNatives() {
#ifdef Q_OS_ANDROID
    QAndroidJniEnvironment env;
    jclass javaClass;

    javaClass = env->FindClass("org/github/nsetzer/example/MyCustomAppActivity");
    if(!javaClass)
        return JNI_ERR;
    if(env->RegisterNatives(javaClass, ActivityArray,
       sizeof(ActivityArray) / sizeof(ActivityArray[0])) < 0)
        return JNI_ERR;


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
