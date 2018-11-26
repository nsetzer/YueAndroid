
/*

unsigned int signalCount = 0;
QEventLoop waitLoop;
const auto signalReceived = [&signalCount,&waitLoop]()->void{
if(--signalCount==0)
waitLoop.quit();
}
connect(sender1, &Sender1::signal1,signalReceived); ++signalCount;
connect(sender2, &Sender2::signal2,signalReceived); ++signalCount;
// ...
connect(senderN, &SenderN::signalN,signalReceived); ++signalCount;
waitLoop.exec();
qDebug("All signals received");

*/
#include "yue/bell/remote/client_test.hpp"

namespace yue {
    namespace bell {
        namespace remote {


QNetworkReply* TestSuccessNetworkManager::get(QNetworkRequest &request) {
    QUrl url = request.url();

    if (url.path() == "/api/user") {
        QString data = "{\"result\":{\"apikey\":\"test-api-key\", \"features\": []}}";
        TestNetworkReply* reply = new TestNetworkReply(url, data.toLocal8Bit());
        reply->setFinished();
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

    auto reply = client.login("http://localhost:4200", "admin", "admin");


}

void TestRemoteClient::test_login()
{

}

QTEST_MAIN(TestRemoteClient)
