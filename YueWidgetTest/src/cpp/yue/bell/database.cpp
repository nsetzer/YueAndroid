
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
    // path:  :memory:
    qDebug() << "connect to database: " << path;
    if (connectionName.isEmpty()) {
        // create the default connection
        m_db = QSqlDatabase::addDatabase("QSQLITE");
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    }

    qDebug() << QSqlDatabase::drivers();

    m_db.setDatabaseName(path);
    if (!m_db.open()) {
        QString err_text = m_db.lastError().text();
        qDebug() << err_text;
        throw std::runtime_error("failed to connect to database");
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
    QSqlQuery q(m_db);

    bool r;
    {
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

    {
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

    {
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

    {
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

    {
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

    {
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
