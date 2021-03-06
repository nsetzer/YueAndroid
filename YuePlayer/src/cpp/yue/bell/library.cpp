
#include <QFileInfo>

#include "yue/bell/library.hpp"
#include "yue/bell/database.hpp"
#include "yue/bell/LibraryTreeNode.hpp"
#include "yue/core/search/grammar.hpp"
#include "yue/core/search/rules.hpp"
#include "yue/core/shuffle.hpp"

#include <algorithm>

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
        return q.value(0).toUInt();
    return 0;
}

Database::uid_t Library::insert(QMap<QString,QVariant> data)
{
    if (!m_db->db().transaction()) {
        qCritical() << "unable to create a transaction" << m_db->db().lastError();
        return 0;
    }
    Database::uid_t uid;
    bool result;
    try {
         result = _insert(data, uid);
    } catch (DatabaseError& e) {
        qCritical() << e.what() << m_db->db().lastError();
        m_db->db().rollback();
        return 0;
    } catch (std::exception& ex) {
        qCritical() << "unexpected exception during sql transaction"
                    << ex.what()
                    << m_db->db().lastError();
        m_db->db().rollback();
        return 0;
    } catch (...) {
        qCritical() << "unhandled exception during sql transaction"
                    << m_db->db().lastError();
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

    // calculate the sort key if given

    QString artist = data[yue::core::Song::artist].toString();
    QString album = data[yue::core::Song::album].toString();
    QString sortkey;
    if (!data.contains(yue::core::Song::artist_key)) {
        QString art = data[yue::core::Song::artist].toString();
        if (art.toLower().startsWith("the ")) {
            art = art.remove(0, 4);
        }
        sortkey = art;
    } else {
        sortkey = data[yue::core::Song::artist_key].toString();
        data.remove(yue::core::Song::artist_key);
    }

    Database::uid_t iArtistId = _get_or_create_artist_id(artist, sortkey);
    Database::uid_t iAlbumId = _get_or_create_album_id(iArtistId,album);

    data.remove(yue::core::Song::artist);
    data.remove(yue::core::Song::album);

    if (data.contains(yue::core::Song::uid))
        data.remove(yue::core::Song::uid);

    QSqlQuery q(m_db->db());
    q.prepare("UPDATE artists SET count=count+1 WHERE uid=?");
    q.addBindValue(toQVariant(iArtistId));
    if(!q.exec()) {
        qWarning() << q.lastError();
        return false;
    }
    q.finish();

    q.prepare("UPDATE albums SET count=count+1 WHERE uid=?");
    q.addBindValue(toQVariant(iAlbumId));
    if(!q.exec()) {
        qWarning() << q.lastError();
        return false;
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
        new_uid = q.lastInsertId().toUInt();
        q.finish();
    } else {
        qWarning() << q.executedQuery();
        qWarning() << q.lastError();
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
        qCritical() << e.what() << m_db->db().lastError();
        m_db->db().rollback();
        return;
    } catch (std::exception& ex) {
        qCritical() << "unexpected exception during sql transaction"
                    << ex.what()
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
    old_artist_id = q.value(0).toUInt();
    old_album_id = q.value(1).toUInt();
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

    m_db->db().transaction();

    QSqlQuery q(m_db->db());
    q.prepare("UPDATE songs SET playcount=playcount+1 WHERE uid=?");
    q.addBindValue(toQVariant(uid));

    if (!q.exec()) {
        qWarning() << "error executing query" << q.lastError();
    }

    m_db->db().commit();
    return;
}

void Library::setRating(Database::uid_t uid, int rating)
{
    m_db->db().transaction();

    QSqlQuery q(m_db->db());
    q.prepare("UPDATE songs SET rating=? WHERE uid=?");
    q.addBindValue(rating);
    q.addBindValue(toQVariant(uid));

    if (!q.exec()) {
        qWarning() << "error executing query" << q.lastError();
    }

    m_db->db().commit();
    return;
}

/**
 * @brief Library::exists
 * @param path to a local file on disk
 * @return true if the path exists in the database
 */
bool Library::exists(QString path)
{
    QSqlQuery query = m_grammar.buildQuery(QStringList() << yue::core::Song::path,
              std::unique_ptr<yue::core::SearchRule>(
                  new yue::core::ExactSearchRule<std::string>(
                      yue::core::Song::path, path.toUtf8().toStdString())),
              "",
              m_db->db());

    if (!query.exec()) {
        qWarning() << "error executing query" << query.lastError();
        return false;
    }

    while (query.next()) {
        return true;
    }

    return false;
}

void Library::remove(QMap<QString,QVariant> data)
{
    m_db->db().transaction();
    bool result;
    try {
         result = _remove(data);
    } catch (DatabaseError& e) {
        qCritical() << e.what() << m_db->db().lastError();
        m_db->db().rollback();
        return;
    } catch (std::exception& ex) {
        qCritical() << "unexpected exception during sql transaction"
                    << ex.what()
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

bool Library::_remove(QMap<QString,QVariant> data)
{
    if (!data.contains(yue::core::Song::artist))
        throw std::runtime_error(std::string("missing key: ") + yue::core::Song::artist);
    if (!data.contains(yue::core::Song::album))
        throw std::runtime_error(std::string("missing key: ") + yue::core::Song::album);
    if (!data.contains(yue::core::Song::uid))
        throw std::runtime_error(std::string("missing key: ") + yue::core::Song::uid);

    QString artist = data[yue::core::Song::artist].toString();
    QString album = data[yue::core::Song::album].toString();
    Database::uid_t iSongId = data[yue::core::Song::uid].toUInt();

    // remove the song from the library
    QSqlQuery query = m_grammar.buildDelete(
              std::unique_ptr<yue::core::SearchRule>(
                  new yue::core::ExactSearchRule<int>(
                      yue::core::Song::uid, static_cast<int>(iSongId))),
              m_db->db());

    if (!query.exec()) {
        qWarning() << query.lastError();
        return false;
    }

    Database::uid_t iArtistId = _get_or_create_artist_id(artist,artist);
    Database::uid_t iAlbumId = _get_or_create_album_id(iArtistId,album);

    // update the artist count

    QSqlQuery q(m_db->db());
    q.prepare("UPDATE artists SET count=count-1 WHERE uid=?");
    q.addBindValue(toQVariant(iArtistId));
    if(!q.exec()) {
        qWarning() << query.lastError();
        return false;
    }
    q.finish();

    // update the album count

    q.prepare("UPDATE albums SET count=count-1 WHERE uid=?");
    q.addBindValue(toQVariant(iAlbumId));
    if(!q.exec()) {
        qWarning() << query.lastError();
        return false;
    }
    q.finish();


    return true;
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
 *
 * @todo memory leak, create Forest type which auto deletes pointers...
 */
QList<LibraryTreeNode*> Library::queryToForest(QString querystr)
{
    LOG_FUNCTION_TIME();

    QList<yue::bell::LibraryTreeNode*> forest;

    typedef yue::core::Song Song;
    QSqlQuery query = m_grammar.buildQuery(QStringList()
                          << Song::artist       // 0
                          << Song::album        // 1
                          << Song::title        // 2
                          << Song::rating       // 3
                          << Song::uid          // 4
                          << Song::artist_key   // 5
                          << Song::album_index, // 6
                          querystr,
                          "artist_key COLLATE NOCASE, "
                          "album COLLATE NOCASE, "
                          "album_index, "
                          "title COLLATE NOCASE ASC");

    if (!query.exec()) {
        qWarning() << "error executing query";
        qWarning() << query.lastError();
        return forest;
    }

    yue::bell::LibraryTreeNode* nd_art = nullptr;
    yue::bell::LibraryTreeNode* nd_alb = nullptr;
    while (query.next()) {

        QString artist = query.value(0).toString();
        QString album  = query.value(1).toString();
        QString title  = query.value(2).toString();
        int rating = query.value(3).toInt();
        Database::uid_t uid = query.value(4).toULongLong();

        if (nd_art == nullptr || nd_art->text() != artist) {
            nd_art = new yue::bell::LibraryTreeNode(artist, 0);
            nd_alb = nullptr;
            forest.push_back(nd_art);
        }
        if (nd_alb == nullptr || nd_alb->text() != album) {
            nd_alb = new yue::bell::LibraryTreeNode(album, 1);
            nd_art->addChild(nd_alb);
        }

        yue::bell::LibraryTreeNode* child = new yue::bell::LibraryTreeNode(uid, title, rating, 2);
        nd_alb->addChild(child);
    }

    return forest;
}
/*
struct pl_elem_t {
    Database::uid_t uid;
    std::string str;
};
*/
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
    if (query == "") {
        q.prepare("SELECT uid, artist FROM library");
    } else {
        q = m_grammar.buildQuery(
                    QStringList() << yue::core::Song::uid << yue::core::Song::artist,
                    query, "", m_db->db());
    }
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
        Database::uid_t uid = q.value(0).toUInt();
        QString group = q.value(1).toString();
        data.push_back( uid );
        groups[uid] = group;
    }


    // shuffle the results

    /*
     TODO: support index order, instead of shuffled
     (Song.year,Song.asc),
     (Song.album,Song.asc),
     (Song.album_index,Song.asc)]
    */
    auto lst = Library::shuffle(data,groups);
    if (size>0 && lst.size()>size) {
        lst.erase(lst.begin()+size,lst.end());
    }
    return lst;
}

namespace {

    struct SortItem {
        int year;
        int index;
        QString album;
        QString title;
    };
}

void Library::sort(QList<Database::uid_t>& songs)
{
    QMap<Database::uid_t,SortItem> itemMap;
    QSqlQuery q(m_db->db());
    q.prepare("SELECT uid, year, album, album_index, title FROM library");
    q.exec();
    if (q.lastError().isValid())
        qWarning() << q.lastError();
    while (q.next()) {
        Database::uid_t uid = q.value(0).toUInt();
        int year = q.value(1).toInt();
        QString album = q.value(2).toString();
        int index = q.value(3).toInt();
        QString title = q.value(4).toString();

        itemMap[uid] = SortItem{year, index, album, title};
    }

    qDebug() << songs.size();
    std::sort(songs.begin(),songs.end(),[&itemMap](Database::uid_t a, Database::uid_t b){
        if (!itemMap.contains(a) || !itemMap.contains(b))
            return false;
        SortItem& itema = itemMap[a];
        SortItem& itemb = itemMap[b];
        if (itema.year != itemb.year) {
            return itema.year < itemb.year;
        } else if (itema.album != itemb.album) {
            return itema.album < itemb.album;
        } else if (itema.index != itemb.index) {
            return itema.index < itemb.index;
        }
        return itema.title < itemb.title;

    });
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

void Library::getDisplayInfo(Database::uid_t uid,QString& artist, QString& album, QString& title)
{
    QSqlQuery q(m_db->db());
    q.prepare("SELECT artist, album, title FROM library WHERE uid=?");
    q.addBindValue( toQVariant(uid) );
    bool result = q.exec();
    if (q.lastError().isValid())
        qWarning() << q.lastError();
    if (result && q.first()) {
        artist = q.value(0).toString();
        album = q.value(1).toString();
        title = q.value(2).toString();
        return;
    }
    throw std::runtime_error("failed to find uid");
}

void Library::getArtInfo(Database::uid_t uid, Database::uid_t& artist_id, Database::uid_t& album_id, QString& path)
{
    QSqlQuery q(m_db->db());
    q.prepare("SELECT artist, album, path FROM songs WHERE uid=?");
    q.addBindValue( toQVariant(uid) );
    bool result = q.exec();
    if (q.lastError().isValid())
        qWarning() << q.lastError();
    if (result && q.first()) {
        artist_id = q.value(0).toUInt();
        album_id = q.value(1).toUInt();
        path = q.value(2).toString();
        return;
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
    if (q.lastError().isValid()) {
        qWarning() << q.lastError();
    }
    if (result && q.first()) {
        return q.value(0).toUInt();
    }
    q.finish();

    q.clear();

    q.prepare("INSERT into artists (artist, sortkey, count) VALUES (?,?,?)");
    q.addBindValue(name);
    q.addBindValue(sortkey);
    q.addBindValue(0);
    result = q.exec();
    if (q.lastError().isValid()) {
        qWarning() << q.lastError();
    }
    if (!result) {
        throw std::runtime_error("failed to insert artist name");
    }

    Database::uid_t uid = q.lastInsertId().toUInt();
    q.finish();
    return  uid;
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
        return q.value(0).toUInt();
    }
    q.finish();
    q.clear();

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

    Database::uid_t uid = q.lastInsertId().toUInt();
    q.finish();
    return  uid;
}


} // namespace bell
} // namespace yue



