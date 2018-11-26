
#ifndef YUE_BELL_REMOTE_CLIENT_HPP
#define YUE_BELL_REMOTE_CLIENT_HPP

#include <QThread>
#include <QtNetwork>
#include <QUrlQuery>
#include <QUrl>
#include <QDebug>
#include <QJsonDocument>
#include <QSharedPointer>
#include <QList>
#include <QApplication>

// http://doc.qt.io/qt-5/qnetworkaccessmanager.html
// http://doc.qt.io/qt-5/qnetworkrequest.html
namespace yue {
    namespace bell {
        namespace remote {

class AbstractNetworkManager : public QObject
{
    Q_OBJECT

public:
    AbstractNetworkManager(QObject* parent = nullptr)
        : QObject(parent)
        , m_headers()
    {}
    virtual ~AbstractNetworkManager() {}

    virtual QNetworkReply* get(QNetworkRequest &request) = 0;
    virtual QNetworkReply* post(QNetworkRequest &request, QIODevice *data) = 0;
    virtual QNetworkReply* post(QNetworkRequest &request, const QByteArray &data) = 0;
    virtual QNetworkReply* put(QNetworkRequest &request, QIODevice *data) = 0;
    virtual QNetworkReply* put(QNetworkRequest &request, const QByteArray &data) = 0;
    virtual QNetworkReply* deleteResource(QNetworkRequest &request) = 0;

    void setHeader(QString name, QString value) {
        m_headers[name] = value;
    }

    void setBasicAuth(QString username, QString password) {
        QString concatenated = username + ":" + password;
        QByteArray data = concatenated.toLocal8Bit().toBase64();
        setHeader("Authorization", "Basic " + data);
    }

    static void wait(QNetworkReply* reply);
protected:
    QMap<QString, QString> m_headers;

    void applyHeaders(QNetworkRequest &request) {
        for (auto iter = m_headers.constBegin(); iter != m_headers.end(); iter++) {
            qDebug() << iter.key() << iter.value();
            request.setRawHeader(iter.key().toLocal8Bit(), iter.value().toLocal8Bit());
        }
    }


};


class QtNetworkManager : public AbstractNetworkManager
{
    Q_OBJECT

public:
    QtNetworkManager(QObject* parent = nullptr)
        : AbstractNetworkManager(parent)
        , m_manager(new QNetworkAccessManager(this))
    {
        connect(m_manager, &QNetworkAccessManager::sslErrors,
                this, &QtNetworkManager::onSslError);
    }
    virtual ~QtNetworkManager() override {}

    virtual QNetworkReply* get(QNetworkRequest &request) override
    {
        applyHeaders(request);
        return m_manager->get(request);
    }

    virtual QNetworkReply* post(QNetworkRequest &request, QIODevice *data) override
    {
        applyHeaders(request);
        return m_manager->post(request, data);
    }

    virtual QNetworkReply* post(QNetworkRequest &request, const QByteArray &data) override
    {
        applyHeaders(request);
        return m_manager->post(request, data);
    }

    virtual QNetworkReply* put(QNetworkRequest &request, QIODevice *data) override
    {
        applyHeaders(request);
        return m_manager->put(request, data);
    }

    virtual QNetworkReply* put(QNetworkRequest &request, const QByteArray &data) override
    {
        for (auto& kv : m_headers) {
            request.setRawHeader(kv.toLocal8Bit(), m_headers[kv].toLocal8Bit());
        }
        return m_manager->put(request, data);
    }

    virtual QNetworkReply* deleteResource(QNetworkRequest &request) override
    {
        for (auto& kv : m_headers) {
            request.setRawHeader(kv.toLocal8Bit(), m_headers[kv].toLocal8Bit());
        }
        return m_manager->deleteResource(request);
    }

private:
    QNetworkAccessManager *m_manager;

    void onSslError(QNetworkReply* reply, QList<QSslError>)
    {
        reply->ignoreSslErrors();
    }
};

class RemoteClient : public QObject {

    Q_OBJECT

public:
    RemoteClient(QObject * parent = nullptr);
    RemoteClient(AbstractNetworkManager* mgr, QObject * parent = nullptr);
    virtual ~RemoteClient();

    QNetworkReply* login(QString hostname, QString username, QString password);
    // on ready read, begin writing to the file
    QNetworkReply* downloadSong(QString song_id, QIODevice* file);

    // begin a process whereby all songs from the server are fetched
    QNetworkReply* fetchLibrary();

private:
    QNetworkReply* fetchLibrary(int page);

signals:
    // emitted after a login attempt,
    // true if we can communicate with the server server
    // andthe username/password is correct
    void connected(bool success);

public slots:

private slots:

    void loginFinished(QNetworkReply* reply);
    void fetchFinished(QNetworkReply*);
    void downloadReadyRead(QNetworkReply* reply, QIODevice* file);
    void downloadFinished(QNetworkReply* reply, QIODevice* file);

    void importSongRecords(const QJsonArray& arr);

private:
    AbstractNetworkManager *m_manager;
    QString m_hostname;


};


        } // remote
    } // bell
} // yue

//Q_DECLARE_METATYPE(yue::bell::remote::RequestFuture);
//Q_DECLARE_METATYPE(yue::bell::remote::ActiveRequest);

#endif

