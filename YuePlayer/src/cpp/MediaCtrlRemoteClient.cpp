#include "MediaCtrlRemoteClient.h"

MediaCtrlRemoteClient::MediaCtrlRemoteClient()
{

    m_repNode.connectToNode(QUrl(QStringLiteral("local:replica")));
    m_rep = QSharedPointer<MediaControlReplica>(m_repNode.acquire<MediaControlReplica>());

    connect(m_rep.data(),&MediaControlReplica::progressChanged,this,&MediaCtrlRemoteClient::onServiceProgressChanged);
    connect(m_rep.data(),&MediaControlReplica::currentIndexChanged,this,&MediaCtrlRemoteClient::onServiceCurrentIndexChanged);

    connect(m_rep.data(),&MediaControlReplica::stateChanged,this,&MediaCtrlRemoteClient::onServiceStateChanged);
    connect(m_rep.data(),&MediaControlReplica::statusChanged,this,&MediaCtrlRemoteClient::onServiceStatusChanged);

    qDebug() << "waiting for RPC source";

    if (m_rep->waitForSource()) {
        qDebug() << "RPC source ready";
    } else {
        qCritical() << "failed to create RPC source";
        //m_rep.clear();
    }

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
    loadCurrentSongInfo();
    emit currentIndexChanged(index);
}

void MediaCtrlRemoteClient::onServiceStatusChanged(int status)
{

    auto s = static_cast<yue::bell::MediaPlayerBase::Status>(status);
    m_status = s;
    emit statusChanged(s);
}

void MediaCtrlRemoteClient::onServiceStateChanged(int state)
{

    auto s = static_cast<yue::bell::MediaPlayerBase::State>(state);
    m_state = s;
    emit stateChanged(s);
}

