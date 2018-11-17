#include "yue/bell/MediaCtrlBackend.h"

namespace yue {
namespace bell {


MediaCtrlBackend::MediaCtrlBackend(QObject *parent/* = nullptr*/)
    : QObject(parent)
    , m_pPlayer(new MediaPlayerQt(this))
    , m_scanner(this)
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
    try {
        auto pl = PlaylistManager::instance()->openCurrent();
        auto pair = pl->current();
        load(pair.first);

        emit currentIndexChanged(static_cast<int>(pair.second));

    } catch (std::runtime_error& e) {
        qWarning() << "Error Loading Song: " << e.what();
    }

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
        emit currentIndexChanged(static_cast<int>(pair.second));
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
        emit currentIndexChanged(static_cast<int>(pair.second));
    } catch (std::runtime_error& e) {
        qWarning() << e.what();
    }
}

void MediaCtrlBackend::setVolume(float volume)
{
    m_pPlayer->setVolume(volume);
}

void MediaCtrlBackend::sync()
{
    auto pl = PlaylistManager::instance()->openCurrent();

    try {
        int index = static_cast<int>(pl->current().second);
        emit currentIndexChanged(index);
    } catch (std::runtime_error& e) {
        qWarning() << e.what();
    }

}

void MediaCtrlBackend::stop()
{
    m_pPlayer->stop();
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
        break;
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

void MediaCtrlBackend::scanStart(QString path)
{
    m_scanner.start(QDir(path));
    emit scanUpdate(m_scanner.isRunning(), 0, 0, 0);
}
void MediaCtrlBackend::scanStop()
{
    m_scanner.stop();
}
void MediaCtrlBackend::scanStatus()
{
    emit scanUpdate(m_scanner.isRunning(), 0, 0, 0);
}


} // bell
} // yue
