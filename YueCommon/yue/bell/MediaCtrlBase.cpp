#include "yue/bell/MediaCtrlBase.h"

namespace yue {
namespace bell {

QSharedPointer<yue::bell::MediaCtrlBase> MediaCtrlBase::m_instance = nullptr;

MediaCtrlBase::MediaCtrlBase(QObject *parent) : QObject(parent)
{

}

void MediaCtrlBase::setCurrentPlaylist(QList<yue::bell::Database::uid_t>& lst)
{
    auto pl = yue::bell::PlaylistManager::instance()->openCurrent();
    pl->set(lst);

    emit playlistReset();
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

void MediaCtrlBase::playNext(int uid)
{
    auto pl = PlaylistManager::instance()->openCurrent();
    auto pair = pl->current();
    pl->insert(pair.second+1,static_cast<Database::uid_t>(uid));
}

void MediaCtrlBase::playSong(int uid)
{
    playNext(uid);
    next();
}

} // bell
} // yue
