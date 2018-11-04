#include "mainwindow.h"
#include <QApplication>
#include <QGuiApplication>
#include <QDebug>
#include <QSharedPointer>

#include "MediaCtrlRemoteClient.h"
#include "MediaCtrlRemoteServer.h"

//#include "yue/bell/database.hpp"
//#include "yue/bell/library.hpp"
//#include "yue/bell/playlist.hpp"
//#include "yue/bell/MediaCtrlBase.h"
//#include "yue/bell/MediaCtrlLocal.h"
//#include "yue/device.h"
//#include "yue/qtcommon/qtcommon.hpp"
//#include "yue/qtcommon/ResourceCache.h"
//#include "yue/bell/LibraryTreeNode.hpp"
//#include "MediaCtrlRemoteClient.h"
//#include "MediaCtrlRemoteServer.h"

//#ifdef Q_OS_ANDROID
//#include <QAndroidJniObject>
//#include <QtAndroid>
//
//#include "JavaCompat.h"
//
//JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*)
//{
//    Q_UNUSED(vm);
//    qDebug() << "JNI_OnLoad";
//    return JavaCompat::RegisterNatives();
//}
//
//#endif

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QtAndroid>

#include "JavaCompat.h"

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*)
{
    Q_UNUSED(vm);
    qDebug() << "JNI_OnLoad";
    return JavaCompat::RegisterNatives();
}

#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QSharedPointer<MainWindow> window;

    QSharedPointer<QRemoteObjectHost> srcNode;
    QSharedPointer<MediaCtrlRemoteServer> mcsvc;
    //QSharedPointer<yue::bell::MediaCtrlBase> mccli;
    QSharedPointer<MediaCtrlRemoteClient> mccli;

    if (QCoreApplication::arguments().count() > 1){
        qDebug() << "service application starting";

        srcNode = QSharedPointer<QRemoteObjectHost>(new QRemoteObjectHost(QUrl(QStringLiteral("local:replica"))));
        mcsvc = MediaCtrlRemoteServer::create();

        srcNode->enableRemoting(mcsvc.data());
        qDebug() << "service created";
    }
    else{

        qDebug() << "client application starting";


#ifdef Q_OS_ANDROID
        mccli = QSharedPointer<MediaCtrlRemoteClient>(new MediaCtrlRemoteClient());
#else
        //mccli = QSharedPointer<yue::bell::MediaCtrlBase>(new yue::bell::MediaCtrlLocal());
#endif
        window = QSharedPointer<MainWindow>(new MainWindow());

        window->show();
    }

    return app.exec();
}
