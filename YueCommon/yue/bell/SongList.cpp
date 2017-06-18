#include "SongList.h"

namespace yue {
namespace bell {

SongList::SongList(QObject *parent)
    : QObject(parent)
    , m_songs()
    , m_current_index()
{

}

void SongList::move(int src, int tgt)
{

}

void SongList::remove(int src)
{

}

void SongList::insert(int idx, Database::uid_t)
{

}

void SongList::push_back(Database::uid_t uid)
{
    m_songs.push_back(uid)
}

// TODO: these two need unit tests for assing/access
Database::uid_t& SongList::operator[](size_t idx)
{
    return m_songs[idx];
}

const Database::uid_t& SongList::operator[](size_t idx) const
{
    return m_songs[idx]
}

} // bell
} // yue
