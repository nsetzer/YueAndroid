#include "MediaCtrlRemoteServer.h"
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QDebug>

MediaCtrlRemoteServer::MediaCtrlRemoteServer()
    : MediaControlSource()
    , m_pBackend(new yue::bell::MediaCtrlBackend(this))
    , m_index(0)
    , m_status(yue::bell::MediaPlayerBase::Status::Unknown)
    , m_state(yue::bell::MediaPlayerBase::State::Unknown)
{
    connect(m_pBackend.data(),&yue::bell::MediaCtrlBackend::progressChanged,
            this,&MediaCtrlRemoteServer::onProgressChanged);

    connect(m_pBackend.data(),&yue::bell::MediaCtrlBackend::currentIndexChanged,
            this,&MediaCtrlRemoteServer::onCurrentIndexChanged);

    connect(m_pBackend.data(),&yue::bell::MediaCtrlBackend::stateChanged,
            this,&MediaCtrlRemoteServer::onStateChanged);

    connect(m_pBackend.data(),&yue::bell::MediaCtrlBackend::statusChanged,
            this,&MediaCtrlRemoteServer::onStatusChanged);

    try {
        m_pBackend->load();
    } catch (std::runtime_error& e) {
        qWarning() << "Error Loading Song: " << e.what();
    }
}

void MediaCtrlRemoteServer::onProgressChanged(float progress)
{
    emit progressChanged(progress);
}
void MediaCtrlRemoteServer::onCurrentIndexChanged(int index)
{
    m_index = index;
    emit currentIndexChanged(index);
    sendNotification();
}
void MediaCtrlRemoteServer::onStatusChanged(yue::bell::MediaPlayerBase::Status status)
{
    m_status = status;
    emit statusChanged(static_cast<int>(status));
}
void MediaCtrlRemoteServer::onStateChanged(yue::bell::MediaPlayerBase::State state)
{
    m_state = state;
    emit stateChanged(static_cast<int>(state));
    sendNotification();
}

void MediaCtrlRemoteServer::sendNotification()
{
#ifdef Q_OS_ANDROID

    // [B  byte array
    // J   long
    QAndroidJniEnvironment env;
    QAndroidJniObject message = QAndroidJniObject::fromString(QString::number(m_index));
    jboolean playing = true;
    // jbyteArray cover = env->NewByteArray(1024);

    //jlong cover_length = 0;


     jbyte a[] = {1,2,3,4,5,6};
     jbyteArray coverart = env->NewByteArray(6);
     env->SetByteArrayRegion(coverart, 0, 6, a);
     //env->GetByteArrayRegion (array, 0, len, reinterpret_cast<jbyte*>(buf));

    QAndroidJniObject::callStaticMethod<void>("org/github/nsetzer/example/MyCustomAppService",
                                       "showNotification",
                                       "([BZLjava/lang/String;)V",
                                       coverart,
                                       playing,
                                       message.object<jstring>()
                                       );

    /*
    QAndroidJniObject::callStaticMethod<void>("org/github/nsetzer/example/MyCustomAppService",
                                       "showNotification2",
                                       "()V");
    */
    qDebug() << "notification sent...";
#endif
}
