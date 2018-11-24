
#include "yue/bell/remote/client.hpp"

#include <QApplication>

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


void RemoteClient::login(QString hostname, QString username, QString password)
{
    m_hostname = hostname;

    QString url = m_hostname + "/api/user";
    QNetworkRequest request(url);

    m_manager->setBasicAuth(username, password);

    QNetworkReply* reply = m_manager->get(request);

    /*
    connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(slotError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
            this, SLOT(slotSslErrors(QList<QSslError>)));
    */

    AbstractNetworkManager::wait(reply);


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
