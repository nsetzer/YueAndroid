
#include "yue/bell/database.hpp"
/*
SqlQuery testQuery(QString("INSERT INTO test(testcol) VALUES(?)"));
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
        SqlQuery query(m_db);
        query.prepare(sql);
        query.exec();

        qDebug() << query.executedQuery();

        if (query.lastError().isValid())
        {
           qDebug() << query.lastQuery() << query.lastError();
        }
        query.finish();
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

    transaction();


    { // Table songs
        SqlQuery q(m_db);
        r=q.exec("CREATE TABLE if not exists songs ( "
                 "uid text PRIMARY KEY, "
                 "file_path text, "
                 "art_path text DEFAULT '', "
                 "artist text, "
                 "artist_key text, "
                 "composer text DEFAULT '', "
                 "album text, "
                 "title text, "
                 "genre text DEFAULT '', "
                 "year integer DEFAULT 0, "
                 "country text DEFAULT '', "
                 "language text DEFAULT '', "
                 "comment text DEFAULT '', "
                 "album_index integer DEFAULT 0, "
                 "length integer DEFAULT 0, "
                 "last_played integer DEFAULT 0, "
                 "date_added integer DEFAULT 0, "
                 "play_count integer DEFAULT 0, "
                 "skip_count integer DEFAULT 0, "
                 "rating integer DEFAULT 0, "
                 "blocked integer DEFAULT 0, "
                 "file_size integer DEFAULT 0, "
                 "remote integer DEFAULT 0, "
                 "FOREIGN KEY(artist) REFERENCES artists(uid), "
                 "FOREIGN KEY(album) REFERENCES albums(uid)"
                 ")");
        if (q.lastError().isValid())
            qWarning() << q.lastError();
        if (!r)
            throw std::runtime_error("failed to create table: songs");
    }

    { // Table playlists
        SqlQuery q(m_db);
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
        SqlQuery q(m_db);
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
        SqlQuery q(m_db);
        r=q.exec("CREATE VIEW IF NOT EXISTS library as "
                 "SELECT uid, file_path, art_path, artist, artist_key, composer,"
                 "album, title, genre, year, country, language, comment, album_index, "
                 "length, last_played, date_added, play_count, skip_count, rating, "
                 "blocked, file_size, remote "
                 "FROM songs "
                 "WHERE remote = 0");
        if (q.lastError().isValid())
            qWarning() << q.lastError() << q.executedQuery();
        if (!r)
            throw std::runtime_error("failed to create view: library");
    }

    commit();
}

bool Database::checked_select(SqlQuery& query)
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

bool Database::transaction()
{
    return m_db.transaction();
}
bool Database::commit()
{
    return m_db.commit();
}
bool Database::rollback()
{
   return  m_db.rollback();
}

} // namespace bell
} // namespace yue
