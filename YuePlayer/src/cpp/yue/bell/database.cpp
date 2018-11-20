
#include "yue/bell/database.hpp"
/*
QSqlQuery testQuery(QString("INSERT INTO test(testcol) VALUES(?)"));
testQuery.bindValue(0, someQStringObg);
testQuery.exec();
*/
/*
QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
db.setDatabaseName(":memory:");
if (!db.open()) {
    QMessageBox::critical(0, qApp->tr("Cannot open database"),
        qApp->tr("Unable to establish a database connection.\n"
                 "This example needs SQLite support. Please read "
                 "the Qt SQL driver documentation for information how "
                 "to build it.\n\n"
                 "Click Cancel to exit."), QMessageBox::Cancel);
    return false;
}
*/

#include <QtDebug>
#include <QFileInfo>

namespace yue {
namespace bell {

QVariant toQVariant(size_t value)
{
    // value can not be implicitly constucted on all linux platforms
    QVariant v;
    v.setValue(value);
    return v;
}



Database* Database::m_instance = nullptr;

Database::Database(QObject *parent/*=nullptr*/)
    : QObject(parent)
{

}

Database::~Database()
{
    m_db.close();
}

QSharedPointer<Database> Database::reconnect(QString connectionName) {

    QSharedPointer<Database> db = QSharedPointer<Database>(new Database());
    db->connect(m_instance->db().databaseName(), connectionName);
    return db;
}

void Database::connect(QString path, QString connectionName)
{

    QFileInfo fi(path);
    qDebug() << "dbconnect path" << path;
    qDebug() << "dbconnect exists"<< fi.exists() << fi.isReadable() << fi.isWritable();
    qDebug() << "dbconnect owner"<< fi.owner();
    qDebug() << "dbconnect size"<< fi.size();
    qDebug() << "dbconnect permissions"<< fi.permissions();

    // path:  :memory:
    qDebug() << "connect to database: " << path;
    if (connectionName.isEmpty()) {
        // create the default connection
        m_db = QSqlDatabase::addDatabase("QSQLITE");
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    }

    m_db.setDatabaseName(path);
    if (!m_db.open()) {
        qDebug() << m_db.lastError();
        throw std::runtime_error("failed to connect to database");
    }

    qDebug() << "dbconnect valid" << m_db.isValid();


    QStringList db_pragma;
    // WAL enables multi-process, multi threading
    db_pragma << "synchronous=NORMAL";
    db_pragma << "journal_mode=WAL";
    //db_pragma << "temp_store=MEMORY";

    for (const QString& pragma : db_pragma) {
        QString sql = QString("PRAGMA %1;").arg(pragma);
        QSqlQuery query(m_db);
        query.prepare(sql);
        query.exec();

        qDebug() << query.executedQuery();

        if (query.lastError().isValid())
        {
           qDebug() << query.lastQuery() << query.lastError();
        }
    }

    create_v1_0();
    /*{
        QMessageBox::critical(0, qApp->tr("Cannot open database"),
            qApp->tr("Unable to establish a database connection.\n"
                     "This example needs SQLite support. Please read "
                     "the Qt SQL driver documentation for information how "
                     "to build it.\n\n"
                     "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
        */
}

void Database::create_v1_0( void )
{
    bool r;

    m_db.transaction();

    { // Table artists
        QSqlQuery q(m_db);
        r=q.exec("CREATE TABLE if not exists artists ( "
                 "uid INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "artist text, "
                 "sortkey text, "
                 "count INTEGER DEFAULT 0 "
                 ")");
        if (q.lastError().isValid())
            qWarning() << q.lastError();
        QString e1 = q.lastError().databaseText();
        if (!r)
            throw std::runtime_error("failed to create table: artists");
    }

    { // Table albums
        QSqlQuery q(m_db);
        r=q.exec("CREATE TABLE if not exists albums ( "
                 "uid INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "artist INTEGER, "
                 "album text, "
                 "sortkey text, "
                 "count INTEGER DEFAULT 0, "
                 "FOREIGN KEY(artist) REFERENCES artists(uid)"
                 ")");
        if (q.lastError().isValid())
            qWarning() << q.lastError();
        if (!r)
            throw std::runtime_error("failed to create table: albums");
    }

    { // Table songs
        QSqlQuery q(m_db);
        r=q.exec("CREATE TABLE if not exists songs ( "
                 "uid integer PRIMARY KEY AUTOINCREMENT, "
                 "path text, "
                 "source_path text DEFAULT '', "
                 "artist INTEGER, "
                 "composer text DEFAULT '', "
                 "album INTEGER, "
                 "title text, "
                 "genre text DEFAULT '', "
                 "year integer DEFAULT 0, "
                 "country text DEFAULT '', "
                 "lang text DEFAULT '', "
                 "comment text DEFAULT '', "
                 "album_index integer DEFAULT 0, "
                 "length integer DEFAULT 0, "
                 "last_played integer DEFAULT 0, "
                 "date_added integer DEFAULT 0, "
                 "playcount integer DEFAULT 0, "
                 "skip_count integer DEFAULT 0, "
                 "rating integer DEFAULT 0, "
                 "blocked integer DEFAULT 0, "
                 "equalizer integer DEFAULT 0, "
                 "opm integer DEFAULT 0, "
                 "frequency integer DEFAULT 0, "
                 "file_size integer DEFAULT 0, "
                 "FOREIGN KEY(artist) REFERENCES artists(uid), "
                 "FOREIGN KEY(album) REFERENCES albums(uid)"
                 ")");
        if (q.lastError().isValid())
            qWarning() << q.lastError();
        if (!r)
            throw std::runtime_error("failed to create table: songs");
    }

    { // Table playlists
        QSqlQuery q(m_db);
        r=q.exec("CREATE TABLE if not exists playlists ( "
                 "uid integer PRIMARY KEY AUTOINCREMENT, "
                 "name text UNIQUE, "
                 "size integer, "
                 "idx integer" // poorly named should be "current_index"
                 ")");
        if (q.lastError().isValid())
            qWarning() << q.lastError();
        if (!r)
            throw std::runtime_error("failed to create table: playlists");
    }

    { // Table playlist_songs
        QSqlQuery q(m_db);
        r=q.exec("CREATE TABLE if not exists playlist_songs ( "
                 "uid integer, "
                 "idx integer, "
                 "song_id integer"
                 ")");
        if (q.lastError().isValid())
            qWarning() << q.lastError();
        if (!r)
            throw std::runtime_error("failed to create table: playlist_songs");
    }

    { // View library
        QSqlQuery q(m_db);
        r=q.exec("CREATE VIEW IF NOT EXISTS library as "
                 "SELECT s.uid, s.path, s.source_path, "
                 "a.artist, a.sortkey as artist_key, "
                 "s.composer, "
                 "b.album, "
                 "s.title, s.genre, s.year, "
                 "s.country, s.lang, s.comment, "
                 "s.album_index, s.length, s.last_played, "
                 "s.date_added, s.playcount, s.skip_count, "
                 "s.rating, s.blocked, s.equalizer, s.opm, "
                 "s.frequency, s.file_size "
                 "FROM songs s, artists a, albums b "
                 "WHERE s.artist=a.uid AND s.album=b.uid");
        if (q.lastError().isValid())
            qWarning() << q.lastError();
        if (!r)
            throw std::runtime_error("failed to create view: library");
    }

    m_db.commit();
}

bool Database::checked_select(QSqlQuery& query)
{
    bool result = query.exec();
    if (query.lastError().isValid()) {
        qCritical() << query.lastError();
    }
    if (result && query.first()) {
        return true;
    }
    return false;
}



} // namespace bell
} // namespace yue
