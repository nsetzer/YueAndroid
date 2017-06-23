#include "SongListModel.h"

namespace yue {
namespace qtcommon {

void MoveUpdateThread::run() {

    m_mutex.lock();
    auto pl = yue::bell::PlaylistManager::instance()->openCurrent();

    while (m_alive) {
        m_cond.wait(&m_mutex);
        QList<yue::bell::Database::uid_t> lst;
        for (auto& item : m_model->getList().elems()) {
            lst.push_back(item.uid );
        }
        pl->set(lst);
        pl->setCurrent(m_model->getList().currentIndex());

        qDebug() << "force update";
        /*for (auto& action : m_actions) {
            pl->move(action.first,action.second);
        }*/

    }
}

void MoveUpdateThread::stop()
{
    {
        QMutexLocker lk(&m_mutex);
        m_alive = false;
        m_cond.wakeOne();
    }

    wait();
}
void MoveUpdateThread::notify()
{
    QMutexLocker lk(&m_mutex);
    m_cond.wakeOne();\
}

void MoveUpdateThread::enqueue(const QList<QPair<int,int>>& actions)
{
    QMutexLocker lk(&m_mutex);
    m_actions.append( actions );
    m_cond.wakeOne();\
}

int SongListModel::SongIdRole = Qt::UserRole+1;
int SongListModel::ArtistRole = Qt::UserRole+2;
int SongListModel::AlbumRole = Qt::UserRole+3;
int SongListModel::TitleRole = Qt::UserRole+4;
int SongListModel::LengthRole = Qt::UserRole+5;

SongListModel::SongListModel(QObject* parent/*= nullptr*/)
    : QAbstractListModel(parent)
    , m_thread(this)
{

    m_mvUpdateTimer.setInterval(250);
    m_mvUpdateTimer.setSingleShot(true);
    connect(&m_mvUpdateTimer,&QTimer::timeout,this,&SongListModel::onCommitMoveActions);

    auto ctrl = yue::bell::MediaCtrlBase::instance( );
    connect(ctrl.data(),&yue::bell::MediaCtrlBase::currentIndexChanged,
            this, &SongListModel::onCtrlIndexChanged);

    m_thread.start();
}

SongListModel::~SongListModel()
{
    m_thread.stop();
}

void SongListModel::setCurrentIndex(int index) {
    // TODO: update playlist, backend
    m_lst.setCurrentIndex(index);
}

void SongListModel::setPlaylistName(QString name) {
    setPlaylist(yue::bell::PlaylistManager::instance()->open(name));
}

void SongListModel::setPlaylist(QSharedPointer<yue::bell::Playlist> playlist)
{
    LOG_FUNCTION_TIME();
    m_playlist = playlist;
    //p.song_id, p.idx, l.artist, l.album, l.title, l.length
    QSqlQuery query = playlist->select();
    query.exec();
    if (query.lastError().isValid())
        qWarning() << query.lastError();
    m_lst.clear();
    while (query.next()) {
        SongData d;
        d.uid = query.value(0).toULongLong();
        d.artist = query.value(2).toString();
        d.album = query.value(3).toString();
        d.title = query.value(4).toString();
        d.length = query.value(5).toInt();
        m_lst.push_back(d);
    }
    m_lst.setCurrentIndex(m_playlist->current().second);
}

int SongListModel::rowCount(const QModelIndex &parent/* = QModelIndex()*/) const
{
    Q_UNUSED(parent);
    return m_lst.size();
}

int SongListModel::columnCount(const QModelIndex &parent/* = QModelIndex()*/) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant SongListModel::data(const QModelIndex &index, int role/* = Qt::DisplayRole*/) const
{
    if (index.row() < 0 || index.row() >= rowCount())
        return QVariant();

    if (role == SongListModel::SongIdRole)
        return m_lst[index.row()].uid;

    if (role == SongListModel::ArtistRole)
        return m_lst[index.row()].artist;

    if (role == SongListModel::AlbumRole)
        return m_lst[index.row()].album;

    if (role == SongListModel::TitleRole)
        return m_lst[index.row()].title;

    if (role == SongListModel::LengthRole)
        return m_lst[index.row()].length;

    return QVariant();
}

QHash<int, QByteArray> SongListModel::roleNames() const
{

    QHash<int, QByteArray> roles(QAbstractItemModel::roleNames());

    roles[SongListModel::SongIdRole] = "uid";
    roles[SongListModel::ArtistRole] = "artist";
    roles[SongListModel::AlbumRole] = "album";
    roles[SongListModel::TitleRole] = "title";
    roles[SongListModel::LengthRole] = "duration";

    return roles;
}

void SongListModel::move(int src, int tgt)
{
    if (src==tgt)
        return;
    if (src < tgt)
        beginMoveRows(QModelIndex(),src,src,QModelIndex(),tgt+1);
    else
        beginMoveRows(QModelIndex(),src,src,QModelIndex(),tgt);

    int cidx = m_lst.currentIndex();
    m_lst.move(src,tgt);

    if (cidx != m_lst.currentIndex())
        emit currentIndexChanged(m_lst.currentIndex());
    //m_mvActions.move(src,tgt);
    m_mvUpdateTimer.start();
    endMoveRows();
}

void SongListModel::remove(int index)
{
    beginRemoveRows(QModelIndex(),index,index);
    int cidx = m_lst.currentIndex();
    m_lst.remove(index);
    if (cidx != m_lst.currentIndex())
        emit currentIndexChanged(m_lst.currentIndex());
    endRemoveRows();
}

void SongListModel::onCommitMoveActions()
{
    m_thread.notify();
    //LOG_FUNCTION_TIME();
    //m_thread.enqueue(m_mvActions.actions());
    //for (const QPair<int,int>& action : m_mvActions.actions()) {
    //    m_playlist->move(action.first,action.second);
    //}
    //m_mvActions.clear();
}

void SongListModel::onCtrlIndexChanged(int idx) {
    m_lst.setCurrentIndex(idx);
    emit currentIndexChanged(idx);
}

} // qtcommon
} // yue
