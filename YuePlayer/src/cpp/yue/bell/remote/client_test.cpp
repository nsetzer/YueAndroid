

#include "yue/bell/remote/client_test.hpp"


void TestRemoteClient::initTestCase()
{
}

void TestRemoteClient::cleanupTestCase()
{
    yue::bell::remote::RemoteClient client;

    client.login("http://localhost:4200", "admin", "admin");
}

void TestRemoteClient::test_login()
{

}

QTEST_MAIN(TestRemoteClient)
