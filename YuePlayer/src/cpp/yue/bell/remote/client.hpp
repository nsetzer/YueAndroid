
#ifndef YUE_BELL_REMOTE_CLIENT_HPP
#define YUE_BELL_REMOTE_CLIENT_HPP

#include <QThread>
#include <QtNetwork>
#include <QUrl>
#include <QDebug>
#include <QJsonDocument>

// http://doc.qt.io/qt-5/qnetworkaccessmanager.html
// http://doc.qt.io/qt-5/qnetworkrequest.html
namespace yue {
    namespace bell {
        namespace remote {


class RemoteClient : public QObject {

    Q_OBJECT

public:
    RemoteClient(QObject * parent = nullptr);
    virtual ~RemoteClient();

    void login(QString hostname, QString username, QString password);

    static void wait(QNetworkReply* reply);

signals:
    void connected();

private:
    QNetworkAccessManager *m_manager;

    QString m_hostname;
    QString m_username;
    QString m_password;



};


        }
    }
}

#endif
