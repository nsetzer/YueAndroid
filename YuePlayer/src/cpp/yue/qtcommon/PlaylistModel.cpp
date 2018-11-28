#include "PlaylistModel.h"


namespace yue {
namespace qtcommon {

int PlaylistModel::SongIdRole = Qt::UserRole+1;
int PlaylistModel::IndexRole = Qt::UserRole+2;
int PlaylistModel::ArtistRole = Qt::UserRole+3;
int PlaylistModel::AlbumRole = Qt::UserRole+4;
int PlaylistModel::TitleRole = Qt::UserRole+5;
int PlaylistModel::LengthRole = Qt::UserRole+6;

PlaylistModel::PlaylistModel(QObject *parent)
    : QSqlQueryModel(parent)
{

}

void PlaylistModel::refresh()
{
    LOG_FUNCTION_TIME();
    if (m_playlist==nullptr) {
        throw std::runtime_error("playlist is null");
    }

    yue::bell::SqlQuery query = m_playlist->select();
    query.exec();
    if (query.lastError().isValid())
        qWarning() << query.lastError();
    setQuery( query );
}

QVariant PlaylistModel::data(const QModelIndex &index, int role/* = Qt::DisplayRole*/) const
{

    QVariant value;

    if(role < Qt::UserRole)
    {
        value = QSqlQueryModel::data(index, role);
    }
    else
    {
        // using the default query:
        // Qt::UserRole + 1 : p.song_id : PlaylistModel::SongIdRole
        // Qt::UserRole + 2 : p.idx     : PlaylistModel::IndexRole
        // Qt::UserRole + 3 : l.artist  : PlaylistModel::ArtistRole
        // Qt::UserRole + 4 : l.album   : PlaylistModel::AlbumRole
        // Qt::UserRole + 5 : l.title   : PlaylistModel::TitleRole
        // Qt::UserRole + 6 : l.length  : PlaylistModel::LengthRole
        int columnIdx = role - Qt::UserRole - 1;
        QModelIndex modelIndex = this->index(index.row(), columnIdx);
        value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
    }

    return value;

}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{

    QHash<int, QByteArray> roles(QAbstractItemModel::roleNames());

    roles[PlaylistModel::SongIdRole] = "song_uid";
    roles[PlaylistModel::IndexRole] = "playlist_index";
    roles[PlaylistModel::ArtistRole] = "artist";
    roles[PlaylistModel::AlbumRole] = "album";
    roles[PlaylistModel::TitleRole] = "title";
    roles[PlaylistModel::LengthRole] = "duration";

    return roles;
}

bool PlaylistModel::move(int src, int tgt)
{
    if (src==tgt)
        return false;
    if (src < tgt)
        beginMoveRows(QModelIndex(),src,src,QModelIndex(),tgt+1);
    else
        beginMoveRows(QModelIndex(),src,src,QModelIndex(),tgt);

    m_playlist->move(src,tgt);
    endMoveRows();
    return true;
}

bool PlaylistModel::removeItem(int row)
{
    beginRemoveRows(QModelIndex(),row,row);
    m_playlist->remove(row);
    endRemoveRows();

    return true;
}



} // qtcommon
} // yue
