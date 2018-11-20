
#include "yue/bell/playlist.hpp"

namespace yue {
namespace bell {

const QString PlaylistManager::current_playlist_name = ":current:";
PlaylistManager* PlaylistManager::m_instance = nullptr;

PlaylistManager::PlaylistManager(Library* library)
    : QObject()
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
    : QObject(/*TODO thread safe parents*/)
    , m_db(library->db())
    , m_plid(plid)
    , m_library(library)
    , m_name(name)
{

}
Playlist::~Playlist()
{

}\

void Playlist::set(QList<Database::uid_t> lst, size_t current_index) {

    LOG_FUNCTION_TIME();

    QSqlQuery q(m_db->db());

    q.exec("BEGIN");

    q.prepare("DELETE FROM playlist_songs WHERE uid=?");
    q.addBindValue(toQVariant(m_plid));
    q.exec();

    size_t idx = 0;
    q.prepare("INSERT into playlist_songs (uid, idx, song_id) VALUES (?,?,?)");
    for (Database::uid_t song_id : lst) {

        q.addBindValue(toQVariant(m_plid));
        q.addBindValue(toQVariant(idx));
        q.addBindValue(toQVariant(song_id));
        q.exec();
        idx++;
    }

    q.prepare("UPDATE playlists SET idx=?,size=? WHERE uid=?");
    q.addBindValue(toQVariant(current_index));
    q.addBindValue(lst.size());
    q.addBindValue(toQVariant(m_plid));
    q.exec();

    q.exec("END");
    return;

}

void Playlist::clear()
{

    QSqlQuery q(m_db->db());
    q.prepare("DELETE FROM playlist_songs WHERE uid=?");
    q.addBindValue(toQVariant(m_plid));
    q.exec();

    q.prepare("UPDATE playlists SET size=0 WHERE uid=?");
    q.addBindValue(toQVariant(m_plid));
    q.exec();
}

size_t Playlist::size() {

    QSqlQuery q(m_db->db());
    q.prepare("SELECT size FROM playlists WHERE uid=?");
    q.addBindValue(toQVariant(m_plid));
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
    q.addBindValue(toQVariant(m_plid));
    q.addBindValue(idx);
    q.exec();
    q.first();
    uid = q.value(0).toULongLong();

    return uid;
}
Database::uid_t Playlist::setCurrent(int idx)
{
    Database::uid_t uid;
    QSqlQuery q(m_db->db());

    q.exec("begin");

    q.prepare("UPDATE playlists SET idx=? WHERE uid=?");
    q.addBindValue(idx);
    q.addBindValue(toQVariant(m_plid));
    q.exec();

    q.prepare("SELECT song_id FROM playlist_songs WHERE uid=? AND idx=?");
    q.addBindValue(toQVariant(m_plid));
    q.addBindValue(idx);
    q.exec();
    q.first();
    uid = q.value(0).toULongLong();

    q.exec("end");
    return uid;
}


QPair<Database::uid_t,size_t> Playlist::current()
{
     Database::uid_t uid;
     size_t index,size;
     QSqlQuery q(m_db->db());

     q.exec("begin");
     q.prepare("SELECT idx,size FROM playlists WHERE uid=?");
     q.addBindValue(toQVariant(m_plid));
     q.exec();
     if (!q.first()) {
         throw std::runtime_error("no current song");
     }
     index = q.value(0).toULongLong();
     size = q.value(1).toULongLong();

     if (size<1 || index>=size) {
         throw std::runtime_error("no current song");
     }

     q.prepare("SELECT song_id FROM playlist_songs WHERE (uid=? AND idx=?)");
     q.addBindValue(toQVariant(m_plid));
     q.addBindValue(toQVariant(index));
     q.exec();
     if (!q.first()) {
         throw std::runtime_error("no current song");
     }
     uid = q.value(0).toULongLong();

     q.exec("end");

     return  QPair<Database::uid_t,size_t>(uid,index);
}

 QPair<Database::uid_t,size_t> Playlist::next()
 {
     Database::uid_t uid;
     size_t index,size;
     QSqlQuery q(m_db->db());

     q.exec("begin");
     q.prepare("SELECT idx,size FROM playlists WHERE uid=?");
     q.addBindValue(toQVariant(m_plid));
     q.exec();
     q.first();
     index = q.value(0).toULongLong();
     size  = q.value(1).toULongLong();

     if (size==0 || index == size-1) {
         throw std::runtime_error("no next song");
     }

     q.prepare("UPDATE playlists SET idx=idx+1 WHERE uid=?");
     q.addBindValue(toQVariant(m_plid));
     q.exec();

     // get the previous song
     index++;
     q.prepare("SELECT song_id FROM playlist_songs WHERE (uid=? AND idx=?)");
     q.addBindValue(toQVariant(m_plid));
     q.addBindValue(toQVariant(index));
     q.exec();
     q.first();
     uid = q.value(0).toULongLong();

     q.exec("end");

     return  QPair<Database::uid_t,size_t>(uid,index);
 }

 QPair<Database::uid_t,size_t> Playlist::prev()
 {
     Database::uid_t uid;
     size_t index,size;
     QSqlQuery q(m_db->db());

     q.exec("begin");
     q.prepare("SELECT idx,size FROM playlists WHERE uid=?");
     q.addBindValue(toQVariant(m_plid));
     q.exec();
     q.first();
     index = q.value(0).toULongLong();
     size  = q.value(1).toULongLong();

     if (index == 0) {
         throw std::runtime_error("no prev song");
     }

     q.prepare("UPDATE playlists SET idx=idx-1 WHERE uid=?");
     q.addBindValue(toQVariant(m_plid));
     q.exec();

     // get the previous song
     index--;
     q.prepare("SELECT song_id FROM playlist_songs WHERE (uid=? AND idx=?)");
     q.addBindValue(toQVariant(m_plid));
     q.addBindValue(toQVariant(index));
     q.exec();
     q.first();
     uid = q.value(0).toULongLong();

     q.exec("end");

     return  QPair<Database::uid_t,size_t>(uid,index);
 }

void Playlist::insert(int idx, Database::uid_t uid)
{
    LOG_FUNCTION_TIME();

    m_db->db().transaction();
    bool result;
    try {
         result = _insert_uid(idx,uid);
    } catch (DatabaseError& e) {
        qCritical() << "database error"
                    << e.what()
                    << m_db->db().lastError();
        m_db->db().rollback();
        return;
    } catch (std::exception& e) {
        qCritical() << "unexpected exception"
                    << e.what()
                    << m_db->db().lastError();
        m_db->db().rollback();
        return;
    } catch (...) {
        qCritical() << "unhandled exception during sql transaction"
                    << m_db->db().lastError();
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
    q.addBindValue(toQVariant(m_plid));
    q.addBindValue(idx);
    q.exec();

    q.prepare("INSERT into playlist_songs (uid,idx,song_id) VALUES (?,?,?)");
    q.addBindValue(toQVariant(m_plid));
    q.addBindValue(toQVariant(static_cast<size_t>(idx)));
    q.addBindValue(toQVariant(uid));
    q.exec();

    q.prepare("UPDATE playlists SET size=size+1 WHERE uid=?");
    q.addBindValue(toQVariant(m_plid));
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
        qCritical() << "database error"
                    << e.what()
                    << m_db->db().lastError();
        m_db->db().rollback();
        return;
    } catch (std::exception& e) {
        qCritical() << "unexpected exception"
                    << e.what()
                    << m_db->db().lastError();
        m_db->db().rollback();
        return;
    } catch (...) {
        qCritical() << "unhandled exception during sql transaction"
                    << m_db->db().lastError();
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
    q.addBindValue(toQVariant(m_plid));
    q.addBindValue(idx);
    q.exec();

    q.prepare("UPDATE playlist_songs SET idx=idx-1 WHERE uid=? and idx>=?");
    q.addBindValue(toQVariant(m_plid));
    q.addBindValue(idx);
    q.exec();

    q.prepare("UPDATE playlists SET size=size-1 WHERE uid=?");
    q.addBindValue(toQVariant(m_plid));
    q.exec();
    qDebug() << "remove index " << idx;
    return true;
}

void Playlist::move(int src, int tgt)
{
    LOG_FUNCTION_TIME();

    m_db->db().transaction();
    bool result;
    try {
         result = _move_one(src,tgt);
    } catch (DatabaseError& e) {
        qCritical() << "database error"
                    << e.what()
                    << m_db->db().lastError();
        m_db->db().rollback();
        return;
    } catch (std::exception& e) {
        qCritical() << "unexpected exception"
                    << e.what()
                    << m_db->db().lastError();
        m_db->db().rollback();
        return;
    } catch (...) {
        qCritical() << "unhandled exception during sql transaction"
                    << m_db->db().lastError();
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
    LOG_FUNCTION_TIME();

    if (src == tgt) {
        return true;
    }

    QList<Database::uid_t> lst = toList();
    //Database::uid_t song_id = lst[src];
    int current_index = static_cast<int>(current().second);

    if (src < 0) {
        return false;
    }

    if (src >= lst.size()) {
        return false;
    }

    if (tgt < 0) {
        tgt = 0;
    }

    if (tgt >= lst.size()) {
        tgt = lst.size()-1;
    }

    if (src < current_index && current_index < tgt) {
        current_index -= 1;
    } else if (src > current_index && tgt < current_index) {
        current_index += 1;
    }

    if (tgt < src) {
        tgt += 1;
    }

    lst.move(src, tgt);
    //lst.removeAt(src);
    //lst.insert(tgt, song_id);
    set(lst, static_cast<size_t>(current_index));


    /*
    int current_index;
    Database::uid_t song_id;
    QSqlQuery q(m_db->db());

    // get the song id from the current playlist at the source index
    q.prepare("SELECT song_id FROM playlist_songs WHERE uid=? AND idx=?");
    q.addBindValue(toQVariant(m_plid));
    q.addBindValue(src);
    q.exec();
    q.first();
    song_id = q.value(0).toULongLong();

    // get the current song index (the song that is playing)
    q.prepare("SELECT idx FROM playlists WHERE (uid=?)");
    q.addBindValue(toQVariant(m_plid));
    q.exec();
    q.first();
    current_index = q.value(0).toInt();
    //qDebug() << src << tgt << current_index;

    if (src < tgt) {
        // if moving the song to a position further down in the list
        // 0 1 2 3 4 5 6 7  // the index order
        // a b c d e f g h  // given this list, move c -> f (2 -> 5)
        // a b   d e f g h  // remove c
        // a b d e f   g h  // subtract 1 for range (2 to 5)
        // a b d e f c g h

        // remove the source index
        q.prepare("DELETE FROM playlist_songs WHERE uid=? AND idx=?");
        q.addBindValue(toQVariant(m_plid));
        q.addBindValue(src);
        q.exec();

        // update all songs above source by subtracting one from the index
        q.prepare("UPDATE playlist_songs SET idx=idx-1 WHERE (uid=? AND (idx>? AND idx<=?))");
        q.addBindValue(toQVariant(m_plid));
        q.addBindValue(src);
        q.addBindValue(tgt);
        q.exec();

        // insert song into the target position
        q.prepare("INSERT into playlist_songs (uid,idx,song_id) VALUES (?,?,?)");
        q.addBindValue(toQVariant(m_plid));
        q.addBindValue(tgt);
        q.addBindValue(toQVariant(song_id));
        q.exec();

        if (src==current_index) {
            // ! something is not right
            q.prepare("UPDATE playlists SET idx=? WHERE (uid=?)");
            q.addBindValue(toQVariant(static_cast<Database::uid_t>(tgt)));
            q.addBindValue(toQVariant(m_plid));
            q.exec();
        } else if (src < current_index && tgt > current_index) {
            q.prepare("UPDATE playlists SET idx=idx-1 WHERE (uid=?)");
            q.addBindValue(toQVariant(m_plid));
            q.exec();
        }
    } else {
        // if moving the song to a position towards the top of the list
        //   0 1 2 3 4 5 6 7 // the list of indices
        //   a b c d e f g h // the list of items
        //                   // move 5 -> 2
        //   a b c d e   g h // remove 5
        //   a b   c d e g h // shift by 1
        //   a b f c d e g h // place back

        q.prepare("DELETE FROM playlist_songs WHERE uid=? AND idx=?");
        q.addBindValue(toQVariant(m_plid));
        q.addBindValue(src);
        q.exec();

        q.prepare("UPDATE playlist_songs SET idx=idx+1 WHERE (uid=? AND (idx>=? AND idx<?))");
        q.addBindValue(toQVariant(m_plid));
        q.addBindValue(tgt);
        q.addBindValue(src);
        q.exec();

        q.prepare("INSERT into playlist_songs (uid,idx,song_id) VALUES (?,?,?)");
        q.addBindValue(toQVariant(m_plid));
        q.addBindValue(tgt+1);
        q.addBindValue(toQVariant(song_id));
        q.exec();

        if (src==current_index) {
            q.prepare("UPDATE playlists SET idx=? WHERE (uid=?)");
            q.addBindValue(toQVariant(static_cast<Database::uid_t>(tgt)));
            q.addBindValue(toQVariant(m_plid));
            q.exec();
        } else if (tgt < current_index && src > current_index) {
            q.prepare("UPDATE playlists SET idx=idx+1 WHERE (uid=?)");
            q.addBindValue(toQVariant(m_plid));
            q.exec();
        }
    }
    */

    return true;
}

QList<Database::uid_t> Playlist::toList() const
{
    QSqlQuery q(m_db->db());
    QList<Database::uid_t> lst;

    q.prepare("SELECT idx, song_id FROM playlist_songs WHERE uid=? ORDER BY idx ASC");
    q.addBindValue(toQVariant(m_plid));

    if (!q.exec()) {
        qWarning() << "error executing query";
        qWarning() << q.lastError();
        return lst;
    }

    if (q.lastError().isValid()) {
        qWarning() << q.lastError();
        return lst;
    }

    while (q.next()) {
        lst.append(q.value(1).toULongLong());
    }

    return lst;
}

/**
 * @brief returns the query used to produce the playlist given a playlist id
 *
 * the select statement returns the information necessary to display
 * the playlist to the user.
 */
QSqlQuery Playlist::select() {
    //m_db->db()
    QSqlQuery q;
    QString s = "SELECT p.song_id, p.idx, l.artist, l.album, l.title, l.length "
            "FROM playlist_songs AS p "
            "INNER JOIN library AS l ON l.uid == p.song_id "
            "WHERE (p.uid=?) "
            "ORDER BY p.idx";
    q.prepare(s);
    q.addBindValue(toQVariant(m_plid));
    return q;

}

} // namespace bell
} // namespace yue
