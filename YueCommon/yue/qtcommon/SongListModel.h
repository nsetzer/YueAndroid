#ifndef YUE_QTCOMMON_SONGLISTMODEL_H
#define YUE_QTCOMMON_SONGLISTMODEL_H

#include <QAbstractListModel>
#include <QTimer>

#include "yue/global.h"
#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
#include "yue/bell/playlist.hpp"
#include "yue/bell/SongList.h"
#include "yue/bell/MediaCtrlBase.h"

namespace yue {
namespace qtcommon {

class YUECOMMON_EXPORT MoveAction : public QObject
{
    Q_OBJECT

public:
    MoveAction()
        : QObject()
    {}
    ~MoveAction() = default;

    void move(int src, int tgt) {
        // QList<QPair<int,int>> m_lst;

        // update the last move
        if (m_lst.size()>0 && m_lst.back().second == src) {
            m_lst.back().second = tgt;
            return;
        }

        // append a new action
        m_lst.push_back(QPair<int,int>(src,tgt));

    }

    const QList<QPair<int,int>>& actions( void ) const {
        return m_lst;
    }

    void clear() {
        m_lst.clear();
    }
private:
    QList<QPair<int,int>> m_lst;

};

class SongListModel;
class MoveUpdateThread : public QThread {
    Q_OBJECT

    QMutex m_mutex;
    QWaitCondition m_cond;
    QList<QPair<int,int>> m_actions;
    bool m_alive=true;
    SongListModel* m_model;

    void run() override;

public:

    MoveUpdateThread(SongListModel* parent)
        : QThread()
        , m_model(parent)
    {}
    void notify();

    void enqueue(const QList<QPair<int,int>>& actions);

    void stop();

};

class YUECOMMON_EXPORT SongListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QString playlistName READ playlistName WRITE setPlaylistName NOTIFY playlistChanged)

    class SongData {
    public:
        yue::bell::Database::uid_t uid;
        QString artist;
        QString album;
        QString title;
        int     length;
    };

public:

    static int SongIdRole;
    static int ArtistRole;
    static int AlbumRole;
    static int TitleRole;
    static int LengthRole;

    SongListModel(QObject* parent=nullptr);
    ~SongListModel();

    void setPlaylistName(QString name);
    void setCurrentIndex(int index);
    void setPlaylist(QSharedPointer<yue::bell::Playlist> playlist);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE void move(int src, int tgt);
    Q_INVOKABLE void remove(int index);

    int currentIndex() { return m_lst.currentIndex(); }
    QString playlistName() { return m_playlistName; }

    const yue::bell::SongList<SongData>& getList() const { return m_lst; }

signals:
    void currentIndexChanged(int index);
    void playlistChanged();

private slots:
    void onCommitMoveActions();
    void onCtrlIndexChanged(int idx);
private:
    QString m_playlistName;
    QSharedPointer<yue::bell::Playlist> m_playlist;
    MoveAction m_mvActions;
    yue::bell::SongList<SongData> m_lst;
    QTimer m_mvUpdateTimer;
    MoveUpdateThread m_thread;
};


} // qtcommon
} // yue

#endif // SONGLISTMODEL_H
