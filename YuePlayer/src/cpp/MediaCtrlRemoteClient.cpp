#include "MediaCtrlRemoteClient.h"

MediaCtrlRemoteClient::MediaCtrlRemoteClient()
{

    m_repNode.connectToNode(QUrl(QStringLiteral("local:replica")));
    m_rep = QSharedPointer<MediaControlReplica>(m_repNode.acquire<MediaControlReplica>());

    connect(m_rep.data(),&MediaControlReplica::progressChanged,this,&MediaCtrlRemoteClient::onServiceProgressChanged);
    connect(m_rep.data(),&MediaControlReplica::currentIndexChanged,this,&MediaCtrlRemoteClient::onServiceCurrentIndexChanged);

    connect(m_rep.data(),&MediaControlReplica::stateChanged,this,&MediaCtrlRemoteClient::onServiceStatusChanged);
    connect(m_rep.data(),&MediaControlReplica::statusChanged,this,&MediaCtrlRemoteClient::onServiceStateChanged);

    qDebug() << "waiting for RPC source";
    bool res = m_rep->waitForSource();
    Q_ASSERT(res);

}

void MediaCtrlRemoteClient::load()
{
    m_rep->load();
}

void MediaCtrlRemoteClient::loadIndex(int index)
{
    m_rep->loadIndex(index);
}

void MediaCtrlRemoteClient::playIndex(int index)
{
    m_rep->playIndex(index);
}

void MediaCtrlRemoteClient::playpause()
{
    m_rep->playpause();
}

void MediaCtrlRemoteClient::next()
{
    m_rep->next();
}

void MediaCtrlRemoteClient::prev()
{
    m_rep->prev();
}

void MediaCtrlRemoteClient::setVolume(float volume)
{
    m_rep->setVolume(volume);
}

void MediaCtrlRemoteClient::setProgress(float progress)
{
    m_rep->setProgress(progress);
}

void MediaCtrlRemoteClient::onServiceProgressChanged(float progress)
{
    emit progressChanged(progress);
}

void MediaCtrlRemoteClient::onServiceCurrentIndexChanged(int index)
{
    emit currentIndexChanged(index);
}

void MediaCtrlRemoteClient::onServiceStatusChanged(int status)
{
    emit statusChanged(static_cast<yue::bell::MediaPlayerBase::Status>(status));
}

void MediaCtrlRemoteClient::onServiceStateChanged(int state)
{
    emit stateChanged(static_cast<yue::bell::MediaPlayerBase::State>(state));
}

