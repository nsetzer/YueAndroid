#include "mainwindow.h"
#include <QApplication>
#include <QGuiApplication>
#include <QDebug>
#include <QSharedPointer>

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

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;

    w.show();

    return app.exec();
}
