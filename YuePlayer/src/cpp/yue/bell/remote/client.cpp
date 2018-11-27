
#include "yue/bell/remote/client.hpp"

#include "yue/bell/library.hpp"
#include "yue/bell/database.hpp"
#include "yue/core/song.hpp"

namespace yue {
    namespace bell {
        namespace remote {


void AbstractNetworkManager::wait(QNetworkReply* reply)
{
    while (!reply->isFinished()) {
        QApplication::processEvents();
        QThread::msleep(100);
    }
}

RemoteClient::RemoteClient(QObject *parent)
    : QObject(parent)
    , m_manager(new QtNetworkManager(this))
    , m_hostname()
{

}

RemoteClient::RemoteClient(AbstractNetworkManager *mgr, QObject *parent)
    : QObject(parent)
    , m_manager(mgr)
    , m_hostname()
{



}


RemoteClient::~RemoteClient()
{

}


QNetworkReply* RemoteClient::login(QString hostname, QString username, QString password)
{
    m_hostname = hostname;

    QString url = m_hostname + "/api/user";
    QNetworkRequest request(url);

    m_manager->setBasicAuth(username, password);

    QNetworkReply* reply = m_manager->get(request);

    connect(reply, &QNetworkReply::finished,
            [=](){this->loginFinished(reply);});

    return reply;

}

QNetworkReply* RemoteClient::downloadSong(QString song_id, QIODevice* file)
{
    if (!file->isOpen() && file->isWritable() &&
            ((file->openMode()&QIODevice::Text) == 0)) {

        throw std::runtime_error("device not open for binary writing");
    }

    QString url = m_hostname + "/api/library/" + song_id;
    QNetworkRequest request(url);

    QNetworkReply* reply = m_manager->get(request);

    connect(reply, &QNetworkReply::finished,
            [=](){this->downloadFinished(reply, file);});
    connect(reply, &QNetworkReply::readyRead,
            [=](){this->downloadReadyRead(reply, file);});

    return reply;
}

QNetworkReply* RemoteClient::fetchLibrary()
{
    return fetchLibrary(0);
}

QNetworkReply* RemoteClient::fetchLibrary(int page)
{
    QUrl url = QUrl(m_hostname + "/api/library");

    QUrlQuery query;
    query.addQueryItem("query", "");
    query.addQueryItem("page", QString::number(page));
    query.addQueryItem("orderby", "id");
    query.addQueryItem("showBanished", "false");
    query.addQueryItem("limit", "250");

    url.setQuery(query);

    QNetworkRequest request(url);
    QNetworkReply* reply = m_manager->get(request);

    connect(reply, &QNetworkReply::finished,
            [=](){this->fetchFinished(reply);});

    return reply;
}

void RemoteClient::loginFinished(QNetworkReply* reply)
{

    if (reply->error()) {
        emit connected(false);
    } else {
        emit connected(true);
    }

    //QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    //if (statusCode.isValid()) {
    //    int status = statusCode.toInt();
    //}

    reply->deleteLater();
}

void RemoteClient::downloadReadyRead(QNetworkReply* reply, QIODevice* file)
{
    // emitted every time new data is available to be read / written
    file->write(reply->readAll());
}

void RemoteClient::downloadFinished(QNetworkReply* reply, QIODevice* file)
{
    // emitted every time new data is available to be read / written
    file->write(reply->readAll());
}

void RemoteClient::fetchFinished(QNetworkReply* reply)
{

    if (reply->error()) {
        //emit connected(false);
    } else {

        int page = -1;

        QUrl url = reply->url();
        if (url.hasQuery()) {
            QUrlQuery query(url.query());
            if (query.hasQueryItem("page")) {
                page = query.queryItemValue("page").toInt();
            }
        }

        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonArray arr = doc.object().find("result").value().toArray();

        importSongRecords(arr);

        // get the next page
        if (page >= 0 && page < 2 && arr.size() > 0) {
            fetchLibrary(page + 1);
        }
    }

    reply->deleteLater();
}

void RemoteClient::importSongRecords(const QJsonArray& arr)
{
    // each object in the array is made up of the following keys
    // id, ref_id, user_id, data_id, file_path, art_path,
    // static_path, artist, artist_key, composer, album, title,
    // genre, year, country, language, comment, album_index, length,
    // last_played, play_count, skip_count, rating, banished, blocked,
    // equalizer, date_added, frequency, file_size

    qDebug() << "processing songs" << arr.size();

    auto lib = yue::bell::Library::instance();

    for (auto iter = arr.constBegin(); iter != arr.constEnd(); iter++) {
        if (!iter->isObject()) {
            continue;
        }
        QJsonObject obj = iter->toObject();

        Database::song_t song;

        //song["id"] = obj.find("id").value().toString();
        //song["static_path"] = obj.find("static_path").value().toString();

        // str
        song[yue::core::Song::artist] = obj.find("artist").value().toString();
        song[yue::core::Song::composer] = obj.find("composer").value().toString();
        song[yue::core::Song::album] = obj.find("album").value().toString();
        song[yue::core::Song::title] = obj.find("title").value().toString();
        song[yue::core::Song::comment] = obj.find("comment").value().toString();
        song[yue::core::Song::genre] = obj.find("genre").value().toString();
        song[yue::core::Song::country] = obj.find("genre").value().toString();
        song[yue::core::Song::lang] = obj.find("language").value().toString();
        // int
        song[yue::core::Song::play_count] = obj.find("play_count").value().toInt();
        song[yue::core::Song::rating] = obj.find("rating").value().toInt();
        song[yue::core::Song::length] = obj.find("length").value().toInt();
        song[yue::core::Song::year] = obj.find("year").value().toInt();
        song[yue::core::Song::album_index] = obj.find("album_index").value().toInt();
        song[yue::core::Song::last_played] = obj.find("last_played").value().toInt();

        // if contains, update
        // otherwise, add uid, and set remote=true in the song
        // before inserting
        //lib->contains(uid)
        //lib->insert(song);
        //lib->update(, data);
    }
}

        }
    }
}
