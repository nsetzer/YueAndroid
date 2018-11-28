
#include <QFileInfo>
#include <QUuid>

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
    SqlQuery q(m_db->db());
    bool result = q.exec("SELECT count(uid) from library");
    if (result && q.first())
        return q.value(0).toUInt();
    return 0;
}

Database::uid_t Library::insert(QMap<QString,QVariant> data)
{
    if (!m_db->transaction()) {

        qCritical() << "unable to create a transaction" << m_db->db().lastError();
        return "";
    }

    if (data.find(yue::core::Song::uid).value().isNull()) {
        data[yue::core::Song::uid] = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }

    bool result;
    try {
         result = _insert(data);
    } catch (DatabaseError& e) {
        qCritical() << e.what() << m_db->db().lastError();
        m_db->rollback();
        return "";
    } catch (std::exception& ex) {
        qCritical() << "unexpected exception during sql transaction"
                    << ex.what()
                    << m_db->db().lastError();
        m_db->rollback();
        return "";
    } catch (...) {
        qCritical() << "unhandled exception during sql transaction"
                    << m_db->db().lastError();
        m_db->rollback();
        return "";
    }

    if (result)
        m_db->commit();
    else
        m_db->rollback();

    return data.find(yue::core::Song::uid).value().toString();
}

bool Library::_insert(QMap<QString,QVariant> data)
{
    if (!data.contains(yue::core::Song::file_path))
        throw std::runtime_error(std::string("missing key: ") +  yue::core::Song::file_path);
    if (!data.contains(yue::core::Song::artist))
        throw std::runtime_error(std::string("missing key: ") + yue::core::Song::artist);
    if (!data.contains(yue::core::Song::album))
        throw std::runtime_error(std::string("missing key: ") + yue::core::Song::album);
    if (!data.contains(yue::core::Song::title))
        throw std::runtime_error(std::string("missing key: ") + yue::core::Song::title);
    if (!data.contains(yue::core::Song::uid))
        throw std::runtime_error(std::string("missing key: ") + yue::core::Song::uid);
    // calculate the sort key if given

    QString artist = data[yue::core::Song::artist].toString();
    QString album = data[yue::core::Song::album].toString();

    // determine the sort key if not provided
    if (!data.contains(yue::core::Song::artist_key)) {
        QString art = data[yue::core::Song::artist].toString();
        if (art.toLower().startsWith("the ")) {
            art = art.remove(0, 4);
        }
        data[yue::core::Song::artist_key] = art;
    }

    QStringList lstkeys;
    QList<QVariant> lstvals;
    QStringList lsttmp;

    for (const char* key : yue::core::Song::StringKeys) {
        if (data.contains(key)) {
            lstkeys << QString(key);
            lstvals << data[key];
            lsttmp << "?";
        }
    }

    for (const char* key : yue::core::Song::IntegerKeys) {
        if (data.contains(key)) {
            lstkeys << QString(key);
            lstvals << data[key];
            lsttmp << "?";
        }
    }

    QString skeys = lstkeys.join(", ");
    QString svals = lsttmp.join(", ");

    SqlQuery q(m_db->db());
    q.prepare("INSERT INTO songs (" + skeys+ ") VALUES (" + svals + ")");
    for (QVariant& v : lstvals)
        q.addBindValue( v );
    if (q.exec()) {
        q.finish();
    } else {
        qWarning() << q.executedQuery();
        qWarning() << q.lastError();
        q.finish();
    }

    return true;
}

void Library::update(Database::uid_t uid, QMap<QString,QVariant> data)
{
    m_db->transaction();
    bool result;
    try {
         result = _update(uid,data);
    } catch (DatabaseError& e) {
        qCritical() << e.what() << m_db->db().lastError();
        m_db->rollback();
        return;
    } catch (std::exception& ex) {
        qCritical() << "unexpected exception during sql transaction"
                    << ex.what()
                    << m_db->db().lastError();
        m_db->rollback();
        return;
    } catch (...) {
        qCritical() << "unhandled exception during sql transaction"
                    << m_db->db().lastError();
        m_db->rollback();
        return;
    }

    if (result)
        m_db->commit();
    else
        m_db->rollback();
    return;
}

bool Library::_update(Database::uid_t uid, QMap<QString,QVariant> data)
{
    bool result;

    if (data.contains(yue::core::Song::uid))
        data.remove(yue::core::Song::uid);
    QStringList lstkeys;
    QList<QVariant> lstvals;

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

    SqlQuery q(m_db->db());
    q.prepare("UPDATE songs SET " + skeys+ " WHERE uid=?");
    for (QVariant& v : lstvals)
        q.addBindValue( v );
    q.addBindValue(uid);

    result = q.exec();
    return result;

}

void Library::incrementPlaycount(Database::uid_t uid)
{
    // TODO: update last_played

    m_db->transaction();

    SqlQuery q(m_db->db());
    q.prepare("UPDATE songs SET playcount=playcount+1 WHERE uid=?");
    q.addBindValue(uid);

    if (!q.exec()) {
        qWarning() << "error executing query" << q.lastError();
    }

    m_db->commit();
    return;
}

void Library::setRating(Database::uid_t uid, int rating)
{
    m_db->transaction();

    SqlQuery q(m_db->db());
    q.prepare("UPDATE songs SET rating=? WHERE uid=?");
    q.addBindValue(rating);
    q.addBindValue(uid);

    if (!q.exec()) {
        qWarning() << "error executing query" << q.lastError() << q.executedQuery();
    }

    m_db->commit();
    return;
}

bool Library::contains(Database::uid_t uid)
{
    //SqlQuery query = m_grammar.buildQuery(QStringList() << yue::core::Song::uid,
    //          std::unique_ptr<yue::core::SearchRule>(
    //              new yue::core::ExactSearchRule<std::string>(
    //                  yue::core::Song::uid, uid.toStdString())),
    //          "",
    //          m_db->db());

    SqlQuery query;
    query.prepare("SELECT uid from songs where uid == ?");
    query.addBindValue(uid);

    if (!query.exec()) {
        query.finish();

        qWarning() << "error executing query" << query.lastError() << query.executedQuery();
        return false;
    }

    while (query.next()) {
        query.finish();
        return true;
    }

    query.finish();
    return false;
}

/**
 * @brief Library::exists
 * @param path to a local file on disk
 * @return true if the path exists in the database
 */
bool Library::exists(QString path)
{
    SqlQuery query = m_grammar.buildQuery(QStringList() << yue::core::Song::file_path,
              std::unique_ptr<yue::core::SearchRule>(
                  new yue::core::ExactSearchRule<std::string>(
                      yue::core::Song::file_path, path.toUtf8().toStdString())),
              "",
              m_db->db());

    if (!query.exec()) {
        query.finish();

        qWarning() << "error executing query" << query.lastError() << query.executedQuery();
        return false;
    }

    while (query.next()) {
        query.finish();
        return true;
    }

    query.finish();
    return false;
}

void Library::remove(QMap<QString,QVariant> data)
{
    m_db->transaction();
    bool result;
    try {
         result = _remove(data);
    } catch (DatabaseError& e) {
        qCritical() << e.what() << m_db->db().lastError();
        m_db->rollback();
        return;
    } catch (std::exception& ex) {
        qCritical() << "unexpected exception during sql transaction"
                    << ex.what()
                    << m_db->db().lastError();
        m_db->rollback();
        return;
    } catch (...) {
        qCritical() << "unhandled exception during sql transaction"
                    << m_db->db().lastError();
        m_db->rollback();
        return;
    }

    if (result)
        m_db->commit();
    else
        m_db->rollback();
    return;
}

bool Library::_remove(QMap<QString,QVariant> data)
{
    if (!data.contains(yue::core::Song::uid))
        throw std::runtime_error(std::string("missing key: ") + yue::core::Song::uid);

    Database::uid_t iSongId = data[yue::core::Song::uid].toString();

    // remove the song from the library
    SqlQuery query = m_grammar.buildDelete(
              std::unique_ptr<yue::core::SearchRule>(
                  new yue::core::ExactSearchRule<std::string>(
                      yue::core::Song::uid, iSongId.toStdString())),
              m_db->db());

    if (!query.exec()) {
        qWarning() << query.lastError();
        return false;
    }

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
    SqlQuery query = m_grammar.buildQuery(QStringList()
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
        qWarning() << "error executing query" << query.lastError() << query.executedQuery();
        return forest;
    }

    yue::bell::LibraryTreeNode* nd_art = nullptr;
    yue::bell::LibraryTreeNode* nd_alb = nullptr;
    while (query.next()) {

        QString artist = query.value(0).toString();
        QString album  = query.value(1).toString();
        QString title  = query.value(2).toString();
        int rating = query.value(3).toInt();
        Database::uid_t uid = query.value(4).toString();

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

QList<LibraryTreeNode*> Library::remoteQueryToForest(Location location, QString querystr)
{
    LOG_FUNCTION_TIME();

    //     << Song::artist       // 0
    //     << Song::album        // 1
    //     << Song::title        // 2
    //     << Song::remote       // 3
    //     << Song::uid          // 4
    //     << Song::artist_key   // 5
    //     << Song::album_index; // 6

    QList<yue::bell::LibraryTreeNode*> forest;

    bool local = (location==Location::Local||location==Location::Both);
    bool remote = (location==Location::Remote||location==Location::Both);
    SqlQuery query = m_grammar.buildRemoteQuery(querystr, local, remote);

    if (!query.exec()) {
        qWarning() << "error executing query" << query.lastError() << query.executedQuery();
        return forest;
    }

    yue::bell::LibraryTreeNode* nd_art = nullptr;
    yue::bell::LibraryTreeNode* nd_alb = nullptr;
    while (query.next()) {

        QString artist = query.value(0).toString();
        QString album  = query.value(1).toString();
        QString title  = query.value(2).toString();
        int remote = query.value(3).toInt();
        Database::uid_t uid = query.value(4).toString();

        if (nd_art == nullptr || nd_art->text() != artist) {
            nd_art = new yue::bell::LibraryTreeNode(artist, 0);
            nd_alb = nullptr;
            forest.push_back(nd_art);
        }
        if (nd_alb == nullptr || nd_alb->text() != album) {
            nd_alb = new yue::bell::LibraryTreeNode(album, 1);
            nd_art->addChild(nd_alb);
        }

        yue::bell::LibraryTreeNode* child = new yue::bell::LibraryTreeNode(uid, title, remote, 2);
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
    SqlQuery q(m_db->db());
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
        Database::uid_t uid = q.value(0).toString();
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
    SqlQuery q(m_db->db());
    q.prepare("SELECT uid, year, album, album_index, title FROM library");
    q.exec();
    if (q.lastError().isValid())
        qWarning() << q.lastError();
    while (q.next()) {
        Database::uid_t uid = q.value(0).toString();
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
    SqlQuery q(m_db->db());
    q.prepare("SELECT path FROM library WHERE uid=?");
    q.addBindValue(uid);
    bool result = q.exec();
    if (q.lastError().isValid())
        qWarning() << q.lastError();
    if (result && q.first()) {
        return q.value(0).toString();
    }
    std::string msg("failed to find uid:");
    msg += uid.toStdString();
    throw std::runtime_error(msg);
}

void Library::getDisplayInfo(Database::uid_t uid,QString& artist, QString& album, QString& title)
{
    SqlQuery q(m_db->db());
    q.prepare("SELECT artist, album, title FROM library WHERE uid=?");
    q.addBindValue(uid);
    bool result = q.exec();
    if (q.lastError().isValid())
        qWarning() << q.lastError();
    if (result && q.first()) {
        artist = q.value(0).toString();
        album = q.value(1).toString();
        title = q.value(2).toString();
        return;
    }
    std::string msg("failed to find uid:");
    msg += uid.toStdString();
    throw std::runtime_error(msg);
}

void Library::getArtInfo(Database::uid_t uid, QString& artist, QString& album, QString& path)
{
    SqlQuery q(m_db->db());
    q.prepare("SELECT artist, album, file_path, art_path FROM songs WHERE uid=?");
    q.addBindValue(uid);
    bool result = q.exec();
    if (q.lastError().isValid())
        qWarning() << q.lastError();
    if (result && q.first()) {
        artist = q.value(0).toString();
        album = q.value(1).toString();
        // TODO: use file_path and guess art_path if art_path is empty
        path = q.value(2).toString();
        //path = q.value(3).toString();
        return;
    }
    std::string msg("failed to find uid:");
    msg += uid.toStdString();
    throw std::runtime_error(msg);
}

} // namespace bell
} // namespace yue



