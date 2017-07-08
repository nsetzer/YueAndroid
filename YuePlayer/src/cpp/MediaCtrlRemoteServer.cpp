#include "MediaCtrlRemoteServer.h"
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QDebug>

#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
#include "yue/bell/playlist.hpp"

QSharedPointer<MediaCtrlRemoteServer> MediaCtrlRemoteServer::m_instance(nullptr);

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

    QString artist, album, title;

    try {
        yue::bell::Database::uid_t uid = yue::bell::PlaylistManager::instance()->openCurrent()->current().first;
        yue::bell::Library::instance()->getDisplayInfo(uid,artist, album, title);
    } catch (std::runtime_error& e) {
        qWarning() << "Error Loading Song Information for notification: " << e.what();
        return;
    }

    QAndroidJniEnvironment env;
    QAndroidJniObject message = QAndroidJniObject::fromString(artist + " - " + title);
    jboolean playing = m_state == yue::bell::MediaPlayerBase::State::Playing;

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

    qDebug() << "notification sent." << artist << title;
#endif
}
