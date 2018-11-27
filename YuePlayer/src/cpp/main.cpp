#include "ui/mainwindow.h"
#ifdef Q_OS_ANDROID
#include <QAndroidService>
#endif
#include <QCoreApplication>
#include <QApplication>
#include <QGuiApplication>
#include <QDebug>
#include <QSharedPointer>

#include "rpc/MediaCtrlRemoteClient.h"
#include "rpc/MediaCtrlRemoteServer.h"

#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
#include "yue/bell/playlist.hpp"
#include "yue/bell/settings.h"
#include "yue/bell/MediaCtrlBase.h"
#include "yue/bell/MediaCtrlLocal.h"
//#include "yue/device.h"
//#include "yue/qtcommon/qtcommon.hpp"
#include "yue/qtcommon/ResourceCache.h"
#include "yue/bell/LibraryTreeNode.hpp"
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

int app_main(QCoreApplication *app, bool isService)
{

    QSharedPointer<MainWindow> window;

    QSharedPointer<QRemoteObjectHost> srcNode;
    QSharedPointer<MediaCtrlRemoteServer> mcsvc;
    QSharedPointer<yue::bell::MediaCtrlBase> mccli;

    qDebug() << "main: create database";
    yue::bell::Database* db = yue::bell::Database::create();

#ifdef Q_OS_ANDROID

    //QString dirPath =yue::qtcommon::Device::getDirectory(yue::qtcommon::Device::DIRECTORY_MUSIC);
    //QDir dirMusic(dirPath);
    //qDebug() << "actual db path" << dirMusic.absoluteFilePath("yue-library.v2.sqlitedb");

    //QString libPath = dirMusic.absoluteFilePath("yue-library.v2.sqlitedb");
    QString libPath = "/mnt/sdcard/Music/yue-library.v2.sqlitedb";

    //QString libPath = ":memory:";
    db->connect(libPath);


#else

    db->connect("./yue-library.v2.sqlitedb");

#endif

    yue::bell::Settings::create();

    qDebug() << "main: create library";
    yue::bell::Library::create();
    qDebug() << "main: create library complete. number of records:" << yue::bell::Library::instance()->size();
    qDebug() << "main: create playlist manager";
    yue::bell::PlaylistManager::create();


    if (isService){
        qDebug() << "service application starting";

        srcNode = QSharedPointer<QRemoteObjectHost>(new QRemoteObjectHost(QUrl(QStringLiteral("local:replica"))));
        mcsvc = MediaCtrlRemoteServer::create();

        srcNode->enableRemoting(mcsvc.data());
        qDebug() << "service created";
    }
    else{

        qDebug() << "client application starting";

       // NOTE: to debug this, open 2 IDEs
       // in Projects > MSVC2015 > Run > add a new runtime confiruration "backend"
       // command line arguments: add '-service' e.g. "$0 -service"
       // in both IDEs click build
       // in one IDE, start the backend
       // in the other, start the frontend
       // *** change the lines below to start a remote client on windows.
#ifdef Q_OS_ANDROID
        mccli = QSharedPointer<yue::bell::MediaCtrlBase>(new MediaCtrlRemoteClient());
#else
        mccli = QSharedPointer<yue::bell::MediaCtrlBase>(new yue::bell::MediaCtrlLocal());
        //mccli = QSharedPointer<yue::bell::MediaCtrlBase>(new MediaCtrlRemoteClient());
#endif
        yue::bell::MediaCtrlBase::registerInstance(mccli);

        {
            auto pl = yue::bell::PlaylistManager::instance()->openCurrent();
            if (pl->size()==0) {
                pl->set( yue::bell::Library::instance()->createPlaylist("", 100) );
            }
            qDebug() << "playlist size " << pl->size();
        }

        window = QSharedPointer<MainWindow>(new MainWindow());

        window->show();
    }

    return app->exec();
}

int main(int argc, char *argv[])
{
    QCoreApplication *app;

    bool isService = argc > 1; // && QString(argv[1]) == "-service";
    QString name = (isService)?"YueBackend_":"YueFrontend";


    // https://stackoverflow.com/questions/48963731/qtservice-automatically-restarts-on-android-after-running-the-application

    if (isService) {
#ifdef Q_OS_ANDROID
        app = new QAndroidService(argc, argv);
        qDebug() << "created android service";
#else
        app = new QCoreApplication(argc, argv);
        qDebug() << "created core service";
#endif
    } else {
        QApplication* gui_app = new QApplication(argc, argv);
        qDebug() << "created application";
        gui_app->setApplicationDisplayName(name);
        gui_app->setDesktopFileName(name);
        app = gui_app;
    }

    qRegisterMetaType<yue::bell::SongInfo>("SongInfo");
    qRegisterMetaType<yue::bell::Database::uid_t>("Database::uid_t");
    qRegisterMetaType<QList<yue::bell::Database::uid_t>>("QList<yue::bell::Database::uid_t>");
    qRegisterMetaType<yue::bell::MediaPlayerBase::State>("MediaPlayerBase::State");
    qRegisterMetaType<yue::bell::MediaPlayerBase::Status>("MediaPlayerBase::Status");
    qRegisterMetaType<yue::qtcommon::ResourceCache::rid_t>("ResourceCache::rid_t");
    qRegisterMetaType<yue::bell::LibraryTreeNode>("LibraryTreeNode");
    qRegisterMetaType<QList<yue::bell::LibraryTreeNode*>>("QList<yue::bell::LibraryTreeNode*>");

    qDebug() << "main: application starting";

    app->setOrganizationName("nsetzer");
    app->setOrganizationDomain("github.com");

    app->setApplicationVersion("1.0");
    app->setApplicationName(name);

    int retval = app_main(app, isService);
    delete app;
    return retval;
}
