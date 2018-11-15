#include "rpc/MediaCtrlRemoteClient.h"

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#endif

MediaCtrlRemoteClient::MediaCtrlRemoteClient()
{

    m_repNode.connectToNode(QUrl(QStringLiteral("local:replica")));
    m_rep = QSharedPointer<MediaControlReplica>(m_repNode.acquire<MediaControlReplica>());

    QObject::connect(m_rep.data(),&MediaControlReplica::progressChanged,
                     this,&MediaCtrlRemoteClient::onServiceProgressChanged);
    QObject::connect(m_rep.data(),&MediaControlReplica::currentIndexChanged,
                     this,&MediaCtrlRemoteClient::onServiceCurrentIndexChanged);

    QObject::connect(m_rep.data(),&MediaControlReplica::stateChanged,
                     this,&MediaCtrlRemoteClient::onServiceStateChanged);
    QObject::connect(m_rep.data(),&MediaControlReplica::statusChanged,
                     this,&MediaCtrlRemoteClient::onServiceStatusChanged);

    QObject::connect(m_rep.data(),&MediaControlReplica::scanUpdate,
                     this,&MediaCtrlRemoteClient::onServiceSyncUpdate);

    qDebug() << "waiting for RPC source";

    if (m_rep->waitForSource()) {
        qDebug() << "RPC source ready";
    } else {
        qCritical() << "failed to create RPC source";
        //m_rep.clear();
    }

    qDebug() << "rep valid: " << m_rep->isInitialized() << m_rep->isReplicaValid();

    loadCurrentSongInfo();
}

void MediaCtrlRemoteClient::load()
{
    if (m_rep)
        m_rep->load();
}

void MediaCtrlRemoteClient::loadIndex(int index)
{
    if (m_rep)
        m_rep->loadIndex(index);
}

void MediaCtrlRemoteClient::playIndex(int index)
{
    if (m_rep)
        m_rep->playIndex(index);
}

void MediaCtrlRemoteClient::playpause()
{
    if (m_rep)
        m_rep->playpause();
}

void MediaCtrlRemoteClient::next()
{
    if (m_rep)
        m_rep->next();
}

void MediaCtrlRemoteClient::prev()
{
    if (m_rep)
        m_rep->prev();
}

void MediaCtrlRemoteClient::sync()
{
    if (m_rep)
        m_rep->sync();
}

void MediaCtrlRemoteClient::setVolume(float volume)
{
    if (m_rep)
        m_rep->setVolume(volume);
}

void MediaCtrlRemoteClient::setProgress(float progress)
{
    if (m_rep)
        m_rep->setProgress(progress);
}

void MediaCtrlRemoteClient::onServiceProgressChanged(float progress)
{
    m_progress = progress;
    emit progressChanged(progress);
}

void MediaCtrlRemoteClient::onServiceCurrentIndexChanged(int index)
{
    qDebug() << "service event: index " << index;
    loadCurrentSongInfo();
    emit currentIndexChanged(index);
}

void MediaCtrlRemoteClient::onServiceStatusChanged(int status)
{
    qDebug() << "service event: status " << status;
    auto s = static_cast<yue::bell::MediaPlayerBase::Status>(status);
    m_status = s;
    emit statusChanged(s);
}

void MediaCtrlRemoteClient::onServiceStateChanged(int state)
{
    qDebug() << "service event: state " << state;
    auto s = static_cast<yue::bell::MediaPlayerBase::State>(state);
    m_state = s;
    emit stateChanged(s);
}

void MediaCtrlRemoteClient::onServiceSyncUpdate(bool running, int ndirs, int nfiles, int nsongs) {
    emit syncUpdate(running, ndirs, nfiles, nsongs);
}


void MediaCtrlRemoteClient::tts()
{
#ifdef Q_OS_ANDROID
    QAndroidJniObject::callStaticMethod<void>("org/github/nsetzer/yueapp2/YueAppActivity",
                                       "promptSpeechInput",
                                       "()V");
#endif
}
