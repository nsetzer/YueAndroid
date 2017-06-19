#include "MediaCtrlLocal.h"

#include "MediaPlayerQt.h"

namespace yue {
namespace bell {


MediaCtrlLocal::MediaCtrlLocal(QObject *parent/* = nullptr*/)
    : MediaCtrlBase(parent)
    , m_pPlayer(nullptr)
{

    m_pPlayer = new MediaPlayerQt(this);

    connect(m_pPlayer,&MediaPlayerBase::progressChanged,
            this,&MediaCtrlLocal::onProgressChanged);

}


MediaCtrlLocal::~MediaCtrlLocal() {
    delete m_pPlayer;
}

void MediaCtrlLocal::load(QString path)
{
    m_pPlayer->load(path);
}

void MediaCtrlLocal::load(Database::uid_t uid)
{
    m_pPlayer->load(Library::instance()->getPath(uid));
}

void MediaCtrlLocal::load()
{
    auto pl = PlaylistManager::instance()->openCurrent();
    auto pair = pl->current();
    load(pair.first);
}

void MediaCtrlLocal::playpause()
{
    m_pPlayer->playpause();
}

void MediaCtrlLocal::next()
{

}

void MediaCtrlLocal::prev()
{

}

void MediaCtrlLocal::setVolume(float volume)
{
    m_pPlayer->setVolume(volume);
}

void MediaCtrlLocal::onProgressChanged(float progress)
{
    emit progressChanged(progress);
}

} // bell
} // yue
