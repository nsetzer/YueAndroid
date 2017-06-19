

#include "yue/bell/library.hpp"
#include "yue/bell/database.hpp"
#include "yue/bell/LibraryTreeNode.hpp"
#include "yue/core/search/grammar.hpp"
#include "yue/core/shuffle.hpp"


namespace yue {
namespace bell {

Library* Library::m_instance = nullptr;

Library::Library(Database *db/*=nullptr*/)
    : QObject(db)
    , m_db(db)
{


}

Library::~Library()
{

}

size_t Library::size()
{
    QSqlQuery q(m_db->db());
    bool result = q.exec("SELECT count(uid) from library");
    if (result && q.first())
        return q.value(0).toULongLong();
    return 0;
}

Database::uid_t Library::insert(QMap<QString,QVariant> data)
{
    m_db->db().transaction();
    Database::uid_t uid;
    bool result;
    try {
         result = _insert(data,uid);
    } catch (DatabaseError& e) {
        qCritical() << e.what();
        qCritical() << m_db->db().lastError();
        m_db->db().rollback();
        return 0;
    } catch (...) {
        qCritical() << "unhandled exception during sql transaction";
        qCritical() << m_db->db().lastError();
        m_db->db().rollback();
        return 0;
    }

    if (result)
        m_db->db().commit();
    else
        m_db->db().rollback();
    return uid;
}

bool Library::_insert(QMap<QString,QVariant> data, Database::uid_t& uid)
{
    if (!data.contains(yue::core::Song::path))
        throw std::runtime_error(std::string("missing key: ") +  yue::core::Song::path);
    if (!data.contains(yue::core::Song::artist))
        throw std::runtime_error(std::string("missing key: ") + yue::core::Song::artist);
    if (!data.contains(yue::core::Song::album))
        throw std::runtime_error(std::string("missing key: ") + yue::core::Song::album);
    if (!data.contains(yue::core::Song::title))
        throw std::runtime_error(std::string("missing key: ") + yue::core::Song::title);

    m_db->db().transaction();

    QString artist = data[yue::core::Song::artist].toString();
    QString album = data[yue::core::Song::album].toString();

    Database::uid_t iArtistId = _get_or_create_artist_id(artist,artist);
    Database::uid_t iAlbumId = _get_or_create_album_id(iArtistId,album);

    data.remove(yue::core::Song::artist);
    data.remove(yue::core::Song::album);

    if (data.contains(yue::core::Song::uid))
        data.remove(yue::core::Song::uid);

    QSqlQuery q(m_db->db());
    q.prepare("UPDATE artists SET count=count+1 WHERE uid=?");
    q.addBindValue(toQVariant(iArtistId));
    if(!q.exec()) {
        m_db->db().rollback();
        throw std::runtime_error("failed to increment artist count");
    }
    q.finish();

    q.prepare("UPDATE albums SET count=count+1 WHERE uid=?");
    q.addBindValue(toQVariant(iAlbumId));
    if(!q.exec()) {
        m_db->db().rollback();
        throw std::runtime_error("failed to increment album count");
    }
    q.finish();

    QStringList lstkeys;
    QList<QVariant> lstvals;
    QStringList lsttmp;

    lstkeys << "artist" << "album";
    lstvals << toQVariant(iArtistId) << toQVariant(iAlbumId);
    lsttmp << "?" << "?";

    for (const char* key : yue::core::Song::StringKeys) {
        if (data.contains(key)) {
            lstkeys << QString(key);
            lstvals << data[key].toString();
            lsttmp << "?";
        }
    }

    for (const char* key : yue::core::Song::IntegerKeys) {
        if (data.contains(key)) {
            lstkeys << QString(key);
            lstvals << data[key].toInt();
            lsttmp << "?";
        }
    }

    QString skeys = lstkeys.join(", ");
    QString svals = lsttmp.join(", ");

    Database::uid_t new_uid = 0;
    q.prepare("INSERT INTO songs (" + skeys+ ") VALUES (" + svals + ")");
    for (QVariant& v : lstvals)
        q.addBindValue( v );
    if (q.exec()) {
        new_uid = q.lastInsertId().toULongLong();
        q.finish();
    } else {
        q.finish();
    }

    uid = new_uid;
    return true;
}


void Library::update(Database::uid_t uid, QMap<QString,QVariant> data)
{
    m_db->db().transaction();
    bool result;
    try {
         result = _update(uid,data);
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

bool Library::_update(Database::uid_t uid, QMap<QString,QVariant> data)
{
    bool artist_updated=false;
    bool album_updated=false;
    Database::uid_t old_artist_id=0;
    Database::uid_t old_album_id=0;
    Database::uid_t new_artist_id=0;
    Database::uid_t new_album_id=0;

    bool result;

    if (data.contains(yue::core::Song::uid))
        data.remove(yue::core::Song::uid);

    QSqlQuery q(m_db->db());
    q.prepare("SELECT artist,album FROM songs WHERE uid=?");
    q.addBindValue(toQVariant(uid));
    result = q.exec();
    if (q.lastError().isValid())
        qWarning() << q.lastError();
    if (!result || !q.first()) {
        m_db->db().rollback();
        throw std::runtime_error("failed to select song");
    }
    old_artist_id = q.value(0).toULongLong();
    old_album_id = q.value(1).toULongLong();
    q.finish();

    if (data.contains(yue::core::Song::artist)) {
        QString artist = data[yue::core::Song::album].toString();
        new_artist_id = _get_or_create_artist_id(artist,artist);

        q.prepare("UPDATE artists SET count=count+1 WHERE uid=?");
        q.addBindValue(toQVariant(new_artist_id));
        result = q.exec();
        if (!result) {
            m_db->db().rollback();
            throw std::runtime_error("failed to update artists");
        }

        q.prepare("UPDATE artists SET count=count-1 WHERE uid=?");
        q.addBindValue(toQVariant(old_artist_id));
        result = q.exec();
        if (!result) {
            m_db->db().rollback();
            throw std::runtime_error("failed to update artists");
        }

        if (!data.contains(yue::core::Song::album)) {
            QSqlQuery q(m_db->db());
            q.prepare("SELECT album FROM albums WHERE uid=?");
            q.addBindValue(toQVariant(old_album_id));
            result = q.exec();
            if (!result || !q.first()) {
                m_db->db().rollback();
                throw std::runtime_error("failed to select song");
            }
            data[yue::core::Song::album] = q.value(0).toString();
        }

        old_artist_id = new_artist_id;
        artist_updated = true;
        data.remove(yue::core::Song::artist);
    }

    if (data.contains(yue::core::Song::album)) {
        QString album = data[yue::core::Song::album].toString();
        new_album_id = _get_or_create_album_id(old_artist_id,album);

        q.prepare("UPDATE albums SET count=count+1 WHERE uid=?");
        q.addBindValue(toQVariant(new_album_id));
        result = q.exec();
        if (!result) {
            m_db->db().rollback();
            throw std::runtime_error("failed to update artists");
        }

        q.prepare("UPDATE albums SET count=count-1 WHERE uid=?");
        q.addBindValue(toQVariant(old_album_id));
        result = q.exec();
        if (!result) {
            m_db->db().rollback();
            throw std::runtime_error("failed to update artists");
        }

        old_album_id = new_album_id;
        album_updated = true;
        data.remove(yue::core::Song::album);
    }

    QStringList lstkeys;
    QList<QVariant> lstvals;

    if (artist_updated) {
        lstkeys << QString(yue::core::Song::artist) + "=?";;
        lstvals << toQVariant(old_artist_id);
    }

    if (album_updated) {
        lstkeys << QString(yue::core::Song::album) + "=?";;
        lstvals << toQVariant(old_album_id);
    }

    for (const char* key : yue::core::Song::StringKeys) {
        if (data.contains(key)) {
            lstkeys << QString(key) + "=?";
            lstvals << data[key].toString();
        }
    }

    for (const char* key : yue::core::Song::IntegerKeys) {
        if (data.contains(key)) {
            lstkeys << QString(key) + "=?";;
            lstvals << data[key].toInt();
        }
    }

    QString skeys = lstkeys.join(", ");

    q.prepare("UPDATE songs SET " + skeys+ " WHERE uid=?");
    for (QVariant& v : lstvals)
        q.addBindValue( v );
    q.addBindValue(toQVariant(uid));

    result = q.exec();
    return result;

}

void Library::incrementPlaycount(Database::uid_t uid)
{
    // TODO: update last_played

    QSqlQuery q(m_db->db());
    q.prepare("UPDATE songs SET playcount=playcount+1 WHERE uid=?");
    q.addBindValue(toQVariant(uid));

    q.exec();
    return;
}

/**
 * @brief Library::queryToForest
 * @param query
 *
 * Build a forest of nodes out of a query result from the library
 * From the query results, the artist will become a root node in
 * the forest sorted by the artist_key. Each artist node will
 * have an album node. The leaves will be titles of the song
 * @return a list of LibraryTreeNode
 */
QList<LibraryTreeNode*> Library::queryToForest(QString querystr)
{
    LOG_FUNCTION_TIME();

    typedef yue::core::Song Song;
    QSqlQuery query = m_grammar.buildQuery(QStringList() << Song::artist
                          << Song::album
                          << Song::title
                          << Song::uid
                          << Song::artist_key
                          << Song::album_index,
                          querystr,
                          "artist_key COLLATE NOCASE, "
                          "album COLLATE NOCASE, "
                          "album_index, "
                          "title COLLATE NOCASE ASC");
    /*
    QString s = "SELECT artist, album, title, uid, artist_key, album_index "
                "FROM library "
                "ORDER BY artist_key COLLATE NOCASE, "
                "album COLLATE NOCASE, "
                "album_index, "
                "title COLLATE NOCASE ASC";
    */

    //QSqlQuery query(s,m_db->db());
    if (!query.exec())
        qWarning() << "error executing query";
    if (query.lastError().isValid())
        qWarning() << query.lastError();
    qDebug() << query.executedQuery();

    QList<yue::bell::LibraryTreeNode*> forest;
    yue::bell::LibraryTreeNode* nd_art = nullptr;
    yue::bell::LibraryTreeNode* nd_alb = nullptr;
    while (query.next()) {

        QString artist = query.value(0).toString();
        QString album  = query.value(1).toString();
        QString title  = query.value(2).toString();
        int uid  = query.value(3).toInt();

        if (nd_art == nullptr || nd_art->text() != artist) {
            nd_art = new yue::bell::LibraryTreeNode(0,artist,0);
            nd_alb = nullptr;
            forest.push_back(nd_art);
        }
        if (nd_alb == nullptr || nd_alb->text() != album) {
            nd_alb = new yue::bell::LibraryTreeNode(0,album,1);
            nd_art->addChild(nd_alb);
        }

        yue::bell::LibraryTreeNode* child = new yue::bell::LibraryTreeNode(uid,title,2);
        nd_alb->addChild(child);
    }

    return forest;
}

struct pl_elem_t {
    Database::uid_t uid;
    std::string str;
};

/**
 * @brief createPlaylist
 * @param query
 * @param size
 * @return
 */
QList<Database::uid_t> Library::createPlaylist(QString query, size_t size/* = 0*/)
{
    LOG_FUNCTION_TIME();
    // TODO: no limit on query
    // use artist and album to prevent repeats
    QSqlQuery q(m_db->db());
    q.prepare("SELECT uid, artist FROM library");
    q.exec();


    /*
     * TODO: hash songs map<string,list>
     * Artist -> list of songs
     *
     * parameter n : the number of random indices to compute in one step
     *               n <= size | map |
     * generate n unique numbers
     *      no duplicates allowed
     *      first number cannot be the same as the last number generated from the previous round
     *          as long as the size of the map > 1
     */

    QList<Database::uid_t> data;
    QMap<Database::uid_t,QString> groups;

    // unpack the query results
    while (q.next()) {
        Database::uid_t uid = q.value(0).toULongLong();
        QString group = q.value(1).toString();
        data.push_back( uid );
        groups[uid] = group;
    }

    // shuffle the results

    auto lst = Library::shuffle(data,groups);
    if (size>0 && lst.size()>size) {
        lst.erase(lst.begin()+size,lst.end());
    }
    return lst;
}

QString Library::getPath(Database::uid_t uid)
{
    QSqlQuery q(m_db->db());
    q.prepare("SELECT path FROM library WHERE uid=?");
    q.addBindValue( toQVariant(uid) );
    bool result = q.exec();
    if (q.lastError().isValid())
        qWarning() << q.lastError();
    if (result && q.first()) {
        return q.value(0).toString();
    }
    throw std::runtime_error("failed to find uid");
}


Database::uid_t Library::_get_or_create_artist_id(QString name, QString sortkey)
{
    bool result;
    QSqlQuery q(m_db->db());
    q.prepare("SELECT uid from artists where artist=?");
    q.addBindValue(name);
    result = q.exec();
    if (q.lastError().isValid())
        qWarning() << q.lastError();
    if (result && q.first()) {
        return q.value(0).toULongLong();
    }
    q.finish();

    q.prepare("INSERT into artists (artist, sortkey, count) VALUES (?,?,?)");
    q.addBindValue(name);
    q.addBindValue(sortkey);
    q.addBindValue(0);
    result = q.exec();
    if (q.lastError().isValid())
        qWarning() << q.lastError();
    if (!result) {
        throw std::runtime_error("failed to insert artist name");
    }

    return  q.lastInsertId().toULongLong();
}

Database::uid_t Library::_get_or_create_album_id(Database::uid_t artist, QString name)
{

    bool result;
    QSqlQuery q(m_db->db());
    q.prepare("SELECT uid from albums where (artist=? AND album=?)");
    q.addBindValue(toQVariant(artist));
    q.addBindValue(name);
    result = q.exec();
    if (q.lastError().isValid())
        qWarning() << q.lastError();
    if (result && q.first()) {
        return q.value(0).toULongLong();
    }
    q.finish();

    q.prepare("INSERT into albums (artist, album, sortkey, count) VALUES (?,?,?,?)");
    q.addBindValue(toQVariant(artist));
    q.addBindValue(name);
    q.addBindValue(name);
    q.addBindValue(0);
    result = q.exec();
    if (q.lastError().isValid())
        qWarning() << q.lastError();
    if (!result) {
        throw std::runtime_error("failed to insert album name");
    }

    return  q.lastInsertId().toULongLong();
}


} // namespace bell
} // namespace yue

