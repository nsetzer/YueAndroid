#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include "yue/global.h"

#include <QSqlQueryModel>
#include "yue/bell/playlist.hpp"

namespace yue {
namespace qtcommon {


/**
 * @brief The PlaylistModel class
 *
 * in practice this tends to be very slow
 */
class YUECOMMON_EXPORT PlaylistModel : public QSqlQueryModel
{
    Q_OBJECT
    Q_PROPERTY(QString playlistName READ playlistName WRITE setPlaylistName NOTIFY playlistChanged)



public:
    static int SongIdRole;
    static int IndexRole;
    static int ArtistRole;
    static int AlbumRole;
    static int TitleRole;
    static int LengthRole;

    PlaylistModel();
    ~PlaylistModel() {}

    void refresh();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    //bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE bool move(int sourceRow, int destinationRow);
    Q_INVOKABLE bool removeItem(int row);

    void setPlaylistName(QString name) {
        setPlaylist(yue::bell::PlaylistManager::instance()->open(name));
    }
    void setPlaylist(QSharedPointer<yue::bell::Playlist> playlist)
    {
        LOG_FUNCTION_TIME();
        m_playlist = playlist;
        QSqlQuery query = playlist->select();
        query.exec();
        if (query.lastError().isValid())
            qWarning() << query.lastError();
        setQuery(query);
        emit playlistChanged();

    }
    QString playlistName() {
        return m_playlist->name();
    }

signals:
    void playlistChanged();

private:
    QString m_playlistName;
    QSharedPointer<yue::bell::Playlist> m_playlist;

};

} // qtcommon
} // yue

#endif // PLAYLISTMODEL_H
