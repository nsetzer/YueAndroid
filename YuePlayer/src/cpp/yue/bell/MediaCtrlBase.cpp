#include "yue/bell/MediaCtrlBase.h"

namespace yue {
namespace bell {

QSharedPointer<yue::bell::MediaCtrlBase> MediaCtrlBase::m_instance = nullptr;

MediaCtrlBase::MediaCtrlBase(QObject *parent)
    : QObject(parent)
{

    connect(this,&MediaCtrlBase::doPlaySong,this,&MediaCtrlBase::playSong);
    connect(this,&MediaCtrlBase::doSetCurrentPlaylist,this,&MediaCtrlBase::setCurrentPlaylist);
}

void MediaCtrlBase::setCurrentPlaylist(QList<yue::bell::Database::uid_t> lst, bool autoplay/* = false*/)
{
    LOG_FUNCTION_TIME();

    auto pl = yue::bell::PlaylistManager::instance()->openCurrent();
    pl->set(lst);

    emit playlistReset();

    if (autoplay) {
        playIndex(0);
    }

    qDebug() << "new playlist set";

}

int MediaCtrlBase::currentIndex()
{
    auto pl = PlaylistManager::instance()->openCurrent();
    int index;
    try {
        auto pair = pl->current();
        index = static_cast<int>(pair.second);
    } catch (std::runtime_error& e) {
        qWarning() << "Error Loading Song: " << e.what();
        index = -1;
    }
    return index;
}

void MediaCtrlBase::playNext(Database::uid_t uid)
{
    auto pl = PlaylistManager::instance()->openCurrent();
    auto pair = pl->current();
    pl->insert(static_cast<int>(pair.second)+1, uid);
    emit playlistReset();
}

void MediaCtrlBase::playSong(Database::uid_t uid)
{
    playNext(uid);
    next();
}

void MediaCtrlBase::loadCurrentSongInfo()
{
    try {
        Database::uid_t uid = PlaylistManager::instance()->openCurrent()->current().first;
        Library::instance()->getDisplayInfo(uid,m_currentSong.m_artist,m_currentSong.m_album,m_currentSong.m_title);
        m_currentSong.m_songid=uid;
    } catch (std::runtime_error& e) {
        qWarning() << "Error Loading Song: " << e.what();
        m_currentSong.m_artist="error";
        m_currentSong.m_album="error";
        m_currentSong.m_title="error";
        m_currentSong.m_songid="";
    }
}

} // bell
} // yue
