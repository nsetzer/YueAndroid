#ifndef YUE_BELL_REMOTE_CLIENT_TEST_HPP
#define YUE_BELL_REMOTE_CLIENT_TEST_HPP

#include <QtTest/QtTest>

#include "yue/bell/remote/client.hpp"

class TestRemoteClient: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void test_login();
    void cleanupTestCase();

};

#endif
