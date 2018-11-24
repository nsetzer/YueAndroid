

#include "yue/bell/remote/client_test.hpp"

namespace yue {
    namespace bell {
        namespace remote {


QNetworkReply* TestSuccessNetworkManager::get(QNetworkRequest &request) {
    QUrl url = request.url();

    if (url.path() == "/api/user") {
        QString data = "{\"result\":{\"apikey\":\"test-api-key\", \"features\": []}}";
        QNetworkReply* reply = new TestNetworkReply(url, data.toLocal8Bit());
        return reply;
    }
    return nullptr;
}
QNetworkReply* TestSuccessNetworkManager::post(QNetworkRequest &request, QIODevice *data) { return nullptr; }
QNetworkReply* TestSuccessNetworkManager::post(QNetworkRequest &request, const QByteArray &data) { return nullptr; }
QNetworkReply* TestSuccessNetworkManager::put(QNetworkRequest &request, QIODevice *data) { return nullptr; }
QNetworkReply* TestSuccessNetworkManager::put(QNetworkRequest &request, const QByteArray &data) { return nullptr; }
QNetworkReply* TestSuccessNetworkManager::deleteResource(QNetworkRequest &request) { return nullptr; }


        } // remote
    } // bell
} // yue


void TestRemoteClient::initTestCase()
{
}

void TestRemoteClient::cleanupTestCase()
{
    yue::bell::remote::TestSuccessNetworkManager mgr;
    yue::bell::remote::RemoteClient client(&mgr, nullptr);

    client.login("http://localhost:4200", "admin", "admin");
}

void TestRemoteClient::test_login()
{

}

QTEST_MAIN(TestRemoteClient)
