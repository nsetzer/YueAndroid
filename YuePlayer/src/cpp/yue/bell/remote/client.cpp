
#include "yue/bell/remote/client.hpp"

#include <QApplication>

namespace yue {
    namespace bell {
        namespace remote {

RemoteClient::RemoteClient(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
    , m_hostname()
    , m_username()
    , m_password()
{

}

RemoteClient::~RemoteClient()
{

}

void RemoteClient::wait(QNetworkReply* reply)
{
    while (!reply->isFinished()) {
        QApplication::processEvents();
        QThread::msleep(100);
    }
}

void RemoteClient::login(QString hostname, QString username, QString password)
{
    m_hostname = hostname;
    m_username = username;
    m_password = password;

    QString url = m_hostname + "/api/user";
    QNetworkRequest request(url);

    // HTTP Basic authentication header value: base64(username:password)
    QString concatenated = username + ":" + password;
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;
    request.setRawHeader("Authorization", headerData.toLocal8Bit());

    QNetworkReply* reply = m_manager->get(request);

    RemoteClient::wait(reply);


    qDebug() << reply->url();

    qint64 size = reply->readBufferSize();

    qDebug() << size;
    //QNetworkReply::NoError;
    qDebug() << reply->error();

    //reply->read()

    // respose format:
    // { "result" : {"apikey": "...", features: [], ...} }

    QByteArray request_data = reply->readAll();

    qDebug() << request_data;

    QJsonDocument doc = QJsonDocument::fromJson(request_data);

    qDebug() << "is null" << doc.isNull()
             << "is empty" << doc.isEmpty()
             << "is object" << doc.isObject()
             << "is array" << doc.isArray();

    QJsonObject obj = doc.object().find("result").value().toObject();

    qDebug() << obj.keys();

    for (const QString& key : obj.keys()) {
        qDebug() << key << obj.find(key).value();
    }



}




        }
    }
}
