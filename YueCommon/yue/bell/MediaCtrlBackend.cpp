#include "yue/bell/MediaCtrlBackend.h"

namespace yue {
namespace bell {


MediaCtrlBackend::MediaCtrlBackend(QObject *parent/* = nullptr*/)
    : QObject(parent)
    , m_pPlayer(new MediaPlayerQt(this))
{

    connect(m_pPlayer.data(),&MediaPlayerBase::progressChanged,
            this,&MediaCtrlBackend::onProgressChanged);

    connect(m_pPlayer.data(),&MediaPlayerBase::statusChanged,
            this,&MediaCtrlBackend::onStatusChanged);

    connect(m_pPlayer.data(),&MediaPlayerBase::stateChanged,
            this,&MediaCtrlBackend::onStateChanged);

}

void MediaCtrlBackend::loadIndex(int index)
{
    auto pl = PlaylistManager::instance()->openCurrent();
    Database::uid_t uid = pl->setCurrent(static_cast<size_t>(index));
    load(uid);
}

void MediaCtrlBackend::load(Database::uid_t uid)
{
    QString path = Library::instance()->getPath(uid);
    m_pPlayer->load(path);
}

void MediaCtrlBackend::load()
{
    auto pl = PlaylistManager::instance()->openCurrent();
    auto pair = pl->current();
    load(pair.first);

}
void MediaCtrlBackend::playIndex(int index)
{
    try {
        loadIndex(static_cast<size_t>(index));
        m_pPlayer->play();
        emit currentIndexChanged(index);
    } catch (std::runtime_error& e) {
        qWarning() << e.what();
    }
}

void MediaCtrlBackend::playpause()
{
    m_pPlayer->playpause();
}

void MediaCtrlBackend::next()
{
    auto pl = PlaylistManager::instance()->openCurrent();
    try {
        auto pair = pl->next();
        load(pair.first);
        m_pPlayer->play();
        emit currentIndexChanged(pair.second);
    } catch (std::runtime_error& e) {
        qWarning() << e.what();
    }
}

void MediaCtrlBackend::prev()
{
    auto pl = PlaylistManager::instance()->openCurrent();
    try {
        auto pair = pl->prev();
        load(pair.first);
        m_pPlayer->play();
        emit currentIndexChanged(pair.second);
    } catch (std::runtime_error& e) {
        qWarning() << e.what();
    }
}

void MediaCtrlBackend::setVolume(float volume)
{
    m_pPlayer->setVolume(volume);
}

void MediaCtrlBackend::setProgress(float progress)
{
    m_pPlayer->setPosition(m_pPlayer->duration() * progress);
}

void MediaCtrlBackend::onProgressChanged(float progress)
{
    emit progressChanged(progress);
}

void MediaCtrlBackend::onCurrentIndexChanged(int index)
{
    emit currentIndexChanged(index);
}

void MediaCtrlBackend::onStatusChanged(MediaPlayerBase::Status status)
{
    switch (status)
    {
    case MediaPlayerBase::Status::Ended:
        onSongEnded();
    default:
        break;
    }

    emit statusChanged(status);
}

void MediaCtrlBackend::onStateChanged(yue::bell::MediaPlayerBase::State state)
{
    emit stateChanged(state);
}

void MediaCtrlBackend::onSongEnded()
{
    next();
}

} // bell
} // yue
