
#include "yue/bell/remote/client.hpp"


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
    qDebug() << "processing songs" << arr.size();

    for (auto iter = arr.constBegin(); iter != arr.constEnd(); iter++) {
        if (!iter->isObject()) {
            continue;
        }
        QJsonObject obj = iter->toObject();

        for (auto iter2 = obj.constBegin(); iter2 != obj.constEnd(); iter2++) {
            qDebug() << iter2.key() << iter2.value();
        }
        break;
    }
}

        }
    }
}
