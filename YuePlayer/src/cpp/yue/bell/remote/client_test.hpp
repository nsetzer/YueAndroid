#ifndef YUE_BELL_REMOTE_CLIENT_TEST_HPP
#define YUE_BELL_REMOTE_CLIENT_TEST_HPP

#include <QtTest/QtTest>

#include "yue/bell/remote/client.hpp"

namespace yue {
    namespace bell {
        namespace remote {

class TestNetworkReply : public QNetworkReply
{
    Q_OBJECT
public:
    TestNetworkReply(QObject* parent = nullptr)
        : QNetworkReply(parent)
        , m_rawData()
        , m_offset(0)
    {
        setFinished(true);
        open(QIODevice::ReadOnly);
    }
    TestNetworkReply(QUrl url, const QByteArray& data, QObject* parent = nullptr)
        : QNetworkReply(parent)
        , m_rawData(data)
        , m_offset(0)
    {
        setUrl(url);
        setFinished(true);
        open(QIODevice::ReadOnly);
    }
    virtual ~TestNetworkReply() override {}

    // returns the number of bytes read, or -1 on error or end of stream
    qint64 readData(char* data, qint64 maxSize) override {
        if (m_offset < m_rawData.size()) {
            qint64 n = qMin(maxSize, m_rawData.size() - m_offset);
            memcpy(data, m_rawData.constData() + m_offset, n);
            m_offset += n;
            return n;
        }
        return -1;
    }

    void setData(const QByteArray& data) {
        m_rawData = data;
        m_offset = 0;
    }

public slots:
    void abort() override {
        return;
    }


    static TestNetworkReply* newError(QUrl url, QNetworkReply::NetworkError err, const QString& str) {
        TestNetworkReply* reply = new TestNetworkReply();
        reply->setUrl(url);
        reply->setError(err, str);
        return reply;
    }
private:
    QByteArray m_rawData;
    qint64 m_offset;

};

// a manager class which always returns a sample success result for a given request
class TestSuccessNetworkManager : public AbstractNetworkManager
{
    Q_OBJECT

public:
    TestSuccessNetworkManager(QObject* parent = nullptr)
        : AbstractNetworkManager(parent)
    {}
    virtual ~TestSuccessNetworkManager() override {}

    virtual QNetworkReply* get(QNetworkRequest &request) override;
    virtual QNetworkReply* post(QNetworkRequest &request, QIODevice *data) override;
    virtual QNetworkReply* post(QNetworkRequest &request, const QByteArray &data) override;
    virtual QNetworkReply* put(QNetworkRequest &request, QIODevice *data) override;
    virtual QNetworkReply* put(QNetworkRequest &request, const QByteArray &data) override;
    virtual QNetworkReply* deleteResource(QNetworkRequest &request) override;


};
        } // remote
    } // bell
} // yue

class TestRemoteClient: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void test_login();
    void cleanupTestCase();

};

#endif
