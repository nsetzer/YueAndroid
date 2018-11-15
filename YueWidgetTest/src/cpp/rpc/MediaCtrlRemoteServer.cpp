#include "rpc/MediaCtrlRemoteServer.h"

#include <QDebug>
#include <QBuffer>
#include <QByteArray>

#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
#include "yue/bell/playlist.hpp"
#include "yue/bell/AlbumArt.h"

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#endif

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

QSharedPointer<MediaCtrlRemoteServer> MediaCtrlRemoteServer::create() {
    m_instance = QSharedPointer<MediaCtrlRemoteServer>(new MediaCtrlRemoteServer());
    return m_instance;
}

QSharedPointer<MediaCtrlRemoteServer> MediaCtrlRemoteServer::instance() {
    return m_instance;
}


void MediaCtrlRemoteServer::onProgressChanged(float progress)
{
    emit progressChanged(progress);
    qDebug() << "server: progress" << progress;
}

void MediaCtrlRemoteServer::onCurrentIndexChanged(int index)
{
    m_index = index;
    emit currentIndexChanged(index);
    qDebug() << "server: index changed" << index;

    try {
        yue::bell::Database::uid_t uid = yue::bell::PlaylistManager::instance()->openCurrent()->current().first;
        QImage art = yue::bell::AlbumArt::getAlbumArt(yue::bell::Library::instance(), uid, QSize(128,128));
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        art.save(&buffer,"PNG");
        m_current_art = ba;

        QString artist, album, title;
        yue::bell::Library::instance()->getDisplayInfo(uid,artist, album, title);
        m_title = title;
        m_message = artist;
    } catch (std::runtime_error& e) {
        qWarning() << "Error Loading Song Information for notification: " << e.what();
        return;
    }

    sendNotification();
}
void MediaCtrlRemoteServer::onStatusChanged(yue::bell::MediaPlayerBase::Status status)
{
    m_status = status;
    emit statusChanged(static_cast<int>(status));
    qDebug() << "server: status changed" << status;
}
void MediaCtrlRemoteServer::onStateChanged(yue::bell::MediaPlayerBase::State state)
{
    m_state = state;
    emit stateChanged(static_cast<int>(state));
    sendNotification();
    qDebug() << "server: state changed" << state;
}

void MediaCtrlRemoteServer::onSyncUpdate(bool running, int ndirs, int nfiles, int nsongs) {
    emit scanUpdate(running, ndirs, nfiles, nsongs);
}

void MediaCtrlRemoteServer::sendNotification()
{
#ifdef Q_OS_ANDROID

    QAndroidJniEnvironment env;
    QAndroidJniObject title = QAndroidJniObject::fromString(m_title);
    QAndroidJniObject message = QAndroidJniObject::fromString(m_message);
    jboolean playing = m_state == yue::bell::MediaPlayerBase::State::Playing;

    jbyteArray coverart = env->NewByteArray(m_current_art.size());
    // char to signed char requires reinterpret cast
    const jbyte *bytes = reinterpret_cast<const jbyte*>(m_current_art.constData());
    env->SetByteArrayRegion(coverart, 0, m_current_art.size(), bytes);

    QAndroidJniObject::callStaticMethod<void>("org/github/nsetzer/yueapp2/YueAppService",
                                       "showNotification",
                                       "([BZLjava/lang/String;Ljava/lang/String;)V",
                                       coverart,
                                       playing,
                                       title.object<jstring>(),
                                       message.object<jstring>()
                                       );

    qDebug() << "notification sent." << m_message;

#endif
}
