
#include "yue/bell/playlist.hpp"

namespace yue {
namespace bell {

const QString PlaylistManager::current_playlist_name = ":current:";
PlaylistManager* PlaylistManager::m_instance = nullptr;

PlaylistManager::PlaylistManager(Library* library)
    : QObject(library)
    , m_library(library)
{

}

PlaylistManager::~PlaylistManager()
{

}

QSharedPointer<Playlist> PlaylistManager::openCurrent()
{
    return open(current_playlist_name);
}

QSharedPointer<Playlist> PlaylistManager::open(QString name)
{
    Database::uid_t plid=0;
    QSqlQuery q(m_library->db()->db());
    q.prepare("SELECT uid FROM playlists WHERE name=?");
    q.addBindValue(name);
    if (!q.exec()) {
        qWarning() << q.lastError();
        throw DatabaseError("Failed to open playlist: " + name);
    }
    if (!q.first()) {
        // create, playlist does not exist
        q.prepare("INSERT into playlists (name, size, idx) VALUES (?,?,?)");
        q.addBindValue(name);
        q.addBindValue(0);
        q.addBindValue(0);
        bool result = q.exec();
        if (q.lastError().isValid())
            qWarning() << q.lastError();
        if (!result) {
            throw std::runtime_error("failed to insert album name");
        }

        plid =  q.lastInsertId().toULongLong();
    } else {
        // playlist exists, extract the id
        plid = q.value(0).toULongLong();
    }

    QSharedPointer<Playlist> playlist = QSharedPointer<Playlist>(new Playlist(m_library, plid, name));
    return playlist;
}


Playlist::Playlist(Library* library, Database::uid_t plid, QString name)
    : QObject(library)
    , m_db(library->db())
    , m_plid(plid)
    , m_library(library)
    , m_name(name)
{

}
Playlist::~Playlist()
{

}\

void Playlist::set(QList<Database::uid_t> lst) {

    LOG_FUNCTION_TIME();

    QSqlQuery q(m_db->db());

    q.exec("BEGIN");

    q.prepare("DELETE FROM playlist_songs WHERE uid=?");
    q.addBindValue(m_plid);
    q.exec();

    size_t idx = 0;
    q.prepare("INSERT into playlist_songs (uid, idx, song_id) VALUES (?,?,?)");
    for (Database::uid_t song_id : lst) {

        q.addBindValue(m_plid);
        q.addBindValue(idx);
        q.addBindValue(song_id);
        q.exec();
        idx++;
    }

    q.prepare("UPDATE playlists SET size=? WHERE uid=?");
    q.addBindValue(lst.size());
    q.addBindValue(m_plid);
    q.exec();

    q.exec("END");
    return;

}

void Playlist::clear()
{

    QSqlQuery q(m_db->db());
    q.prepare("DELETE FROM playlist_songs WHERE uid=?");
    q.addBindValue(m_plid);
    q.exec();

    q.prepare("UPDATE playlists SET size=0 WHERE uid=?");
    q.addBindValue(m_plid);
    q.exec();
}

size_t Playlist::size() {

    QSqlQuery q(m_db->db());
    q.prepare("SELECT size FROM playlists WHERE uid=?");
    q.addBindValue(m_plid);
    q.exec();

    if (!q.first())
        throw DatabaseError("Playlist::size : no database found");

    return q.value(0).toULongLong();
}


Database::uid_t Playlist::get(int idx)
{
    Database::uid_t uid;
    QSqlQuery q(m_db->db());

    q.prepare("SELECT song_id FROM playlist_songs WHERE uid=? AND idx=?");
    q.addBindValue(m_plid);
    q.addBindValue(idx);
    q.exec();
    q.first();
    uid = q.value(0).toULongLong();

    return uid;
}

void Playlist::insert(int idx, Database::uid_t uid)
{
    LOG_FUNCTION_TIME();

    m_db->db().transaction();
    bool result;
    try {
         result = _insert_uid(idx,uid);
    } catch (DatabaseError& e) {
        qCritical() << e.what();
        qCritical() << m_db->db().lastError();
        m_db->db().rollback();
        return;
    } catch (...) {
        qCritical() << "unhandled exception during sql transaction";
        qCritical() << m_db->db().lastError();
        m_db->db().rollback();
        return;
    }

    if (result)
        m_db->db().commit();
    else
        m_db->db().rollback();
    return;
}

bool Playlist::_insert_uid(int idx, Database::uid_t uid)
{
    QSqlQuery q(m_db->db());
    q.prepare("UPDATE playlist_songs SET idx=idx+1 WHERE uid=? and idx>=?");
    q.addBindValue(m_plid);
    q.addBindValue(idx);
    q.exec();

    q.prepare("INSERT into playlist_songs (uid,idx,song_id) VALUES (?,?,?)");
    q.addBindValue(m_plid);
    q.addBindValue(idx);
    q.addBindValue(uid);
    q.exec();

    q.prepare("UPDATE playlists SET size=size+1 WHERE uid=?");
    q.addBindValue(m_plid);
    q.exec();
    return true;
}

void Playlist::remove(int idx)
{
    LOG_FUNCTION_TIME();

    m_db->db().transaction();
    bool result;
    try {
         result = _remove_one(idx);
    } catch (DatabaseError& e) {
        qCritical() << e.what();
        qCritical() << m_db->db().lastError();
        m_db->db().rollback();
        return;
    } catch (...) {
        qCritical() << "unhandled exception during sql transaction";
        qCritical() << m_db->db().lastError();
        m_db->db().rollback();
        return;
    }

    if (result)
        m_db->db().commit();
    else
        m_db->db().rollback();
    return;
}

bool Playlist::_remove_one(int idx)
{
    QSqlQuery q(m_db->db());

    q.prepare("DELETE FROM playlist_songs WHERE uid=? AND idx=?");
    q.addBindValue(m_plid);
    q.addBindValue(idx);
    q.exec();

    q.prepare("UPDATE playlist_songs SET idx=idx-1 WHERE uid=? and idx>=?");
    q.addBindValue(m_plid);
    q.addBindValue(idx);
    q.exec();

    q.prepare("UPDATE playlists SET size=size-1 WHERE uid=?");
    q.addBindValue(m_plid);
    q.exec();
    return true;
}

void Playlist::move(int src, int tgt)
{
    LOG_FUNCTION_TIME();
    qDebug("move %d->%d",src,tgt);

    m_db->db().transaction();
    bool result;
    try {
         result = _move_one(src,tgt);
    } catch (DatabaseError& e) {
        qCritical() << e.what();
        qCritical() << m_db->db().lastError();
        m_db->db().rollback();
        return;
    } catch (...) {
        qCritical() << "unhandled exception during sql transaction";
        qCritical() << m_db->db().lastError();
        m_db->db().rollback();
        return;
    }

    if (result)
        m_db->db().commit();
    else
        m_db->db().rollback();
    return;
}

bool Playlist::_move_one(int src, int tgt)
{

    if (src == tgt) {
        return true;
    }

    Database::uid_t song_id;
    QSqlQuery q(m_db->db());

    q.prepare("SELECT song_id FROM playlist_songs WHERE uid=? AND idx=?");
    q.addBindValue(m_plid);
    q.addBindValue(src);
    q.exec();
    q.first();
    song_id = q.value(0).toULongLong();

    if (src < tgt) {

        q.prepare("DELETE FROM playlist_songs WHERE uid=? AND idx=?");
        q.addBindValue(m_plid);
        q.addBindValue(src);
        q.exec();

        q.prepare("UPDATE playlist_songs SET idx=idx-1 WHERE (uid=? AND (idx>? AND idx<?))");
        q.addBindValue(m_plid);
        q.addBindValue(src);
        q.addBindValue(tgt);
        q.exec();

        q.prepare("INSERT into playlist_songs (uid,idx,song_id) VALUES (?,?,?)");
        q.addBindValue(m_plid);
        q.addBindValue(tgt-1);
        q.addBindValue(song_id);
        q.exec();
    } else {
        q.prepare("DELETE FROM playlist_songs WHERE uid=? AND idx=?");
        q.addBindValue(m_plid);
        q.addBindValue(src);
        q.exec();

        q.prepare("UPDATE playlist_songs SET idx=idx+1 WHERE (uid=? AND (idx>=? AND idx<?))");
        q.addBindValue(m_plid);
        q.addBindValue(tgt);
        q.addBindValue(src);
        q.exec();

        q.prepare("INSERT into playlist_songs (uid,idx,song_id) VALUES (?,?,?)");
        q.addBindValue(m_plid);
        q.addBindValue(tgt);
        q.addBindValue(song_id);
        q.exec();
    }

    return true;
}

/**
 * @brief returns the query used to produce the playlist given a playlist id
 *
 * the select statement returns the information necessary to display
 * the playlist to the user.
 */
QSqlQuery Playlist::select() {
    QSqlQuery q(m_db->db());
    QString s = "SELECT p.song_id, p.idx, l.artist, l.album, l.title, l.length "
            "FROM playlist_songs AS p "
            "INNER JOIN library AS l ON l.uid == p.song_id "
            "WHERE (p.uid=?) "
            "ORDER BY p.idx";
    q.prepare(s);
    q.addBindValue(m_plid);
    return q;

}

} // namespace bell
} // namespace yue
