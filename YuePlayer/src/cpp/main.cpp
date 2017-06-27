#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>
#include <QSharedPointer>

#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
#include "yue/bell/playlist.hpp"
#include "yue/bell/MediaCtrlBase.h"
#include "yue/bell/MediaCtrlLocal.h"
#include "yue/device.h"
#include "yue/qtcommon/qtcommon.hpp"

#include "MediaCtrlRemoteClient.h"
#include "MediaCtrlRemoteServer.h"

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QtAndroid>
#endif

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qRegisterMetaType<yue::bell::MediaPlayerBase::State>("MediaPlayerBase::State");
    qRegisterMetaType<yue::bell::MediaPlayerBase::Status>("MediaPlayerBase::Status");

    QSharedPointer<QRemoteObjectHost> srcNode;
    QSharedPointer<MediaCtrlRemoteServer> mcsvc;
    QSharedPointer<yue::bell::MediaCtrlBase> mccli;


    yue::bell::Database* db = yue::bell::Database::create();

    // all runtime cross thread signal/slots

#ifdef Q_OS_ANDROID

    QString dirPath =yue::qtcommon::Device::getDirectory(yue::qtcommon::Device::DIRECTORY_MUSIC);
    QDir dirMusic(dirPath);
    QString libPath = dirMusic.absoluteFilePath("yue-library.v1.sqlitedb");
    db->connect(libPath);


    //db->connect("/mnt/sdcard/Music/library.db");
    //db->connect("/mnt/sdcard/Music/yue-library.v1.sqlitedb");

#else

#ifdef Q_OS_LINUX
    db->connect("/home/nsetzer/projects/android/YueMusicPlayer/yue.db");
#else
    //db->connect("D:/git/YueMusicPlayer/yue.db");
    db->connect(":memory:");
#endif
#endif

    qDebug() << "main: create library";
    yue::bell::Library::create();
    qDebug() << "main: create playlist manager";
    yue::bell::PlaylistManager::create();

    if (QCoreApplication::arguments().count() > 1){
        qDebug() << "service application starting";
        {
            auto pl = yue::bell::PlaylistManager::instance()->openCurrent();
            if (pl->size()==0) {
                pl->set( yue::bell::Library::instance()->createPlaylist("", 100) );
            }
            qDebug() << "playlist size " << pl->size();
        }

        srcNode = QSharedPointer<QRemoteObjectHost>(new QRemoteObjectHost(QUrl(QStringLiteral("local:replica"))));
        mcsvc = QSharedPointer<MediaCtrlRemoteServer>(new MediaCtrlRemoteServer());

        srcNode->enableRemoting(mcsvc.data());
        qDebug() << "service created";
    }
    else{


        qDebug() << "client application starting";
        yue::qtcommon::registerQmlTypes();

#ifdef Q_OS_ANDROID
        mccli = QSharedPointer<yue::bell::MediaCtrlBase>(new MediaCtrlRemoteClient());
#else
        mccli = QSharedPointer<yue::bell::MediaCtrlBase>(new yue::bell::MediaCtrlLocal());
#endif
        engine.rootContext()->setContextProperty("MediaPlayer", mccli.data());
        yue::qtcommon::Device::create();
        engine.rootContext()->setContextProperty("gDevice", yue::qtcommon::Device::instance());
        yue::bell::MediaCtrlBase::registerInstance( mccli );

        engine.addImportPath(QStringLiteral("qrc:/"));
        engine.load(QUrl(QStringLiteral("qrc:/src/qml/main.qml")));
    }
    return app.exec();
}
