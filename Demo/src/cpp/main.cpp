#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>

#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
#include "yue/bell/playlist.hpp"
#include "yue/bell/MediaCtrlBase.h"
#include "yue/bell/MediaCtrlLocal.h"
#include "yue/qtcommon/qtcommon.hpp"

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QtAndroid>
#endif


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    yue::qtcommon::registerQmlTypes();
    yue::bell::Database* db = yue::bell::Database::create();

    // http://www.helloandroid.com/tutorials/musicdroid-audio-player-part-ii
#ifdef Q_OS_ANDROID
    db->connect("/mnt/sdcard/Music/library.db");

#else

#ifdef Q_OS_LINUX
    db->connect("/home/nsetzer/projects/android/YueMusicPlayer/yue.db");
#else
    db->connect("D:/git/YueMusicPlayer/yue.db");
#endif
#endif

    yue::bell::Library::create();
    yue::bell::PlaylistManager::create();

    {
        auto pl = yue::bell::PlaylistManager::instance()->openCurrent();
        //if (pl->size()==0) {
            pl->set( yue::bell::Library::instance()->createPlaylist("", 100) );
        //}
        qDebug() << "playlist size " << pl->size();
    }


    QSharedPointer<yue::bell::MediaCtrlBase> mplayer(new yue::bell::MediaCtrlLocal());
    engine.rootContext()->setContextProperty("MediaPlayer", mplayer.data());
    yue::bell::MediaCtrlBase::registerInstance( mplayer );
    //mplayer->load("D:\\Music\\Discography\\Discography - Beast\\[2009] Beast\\01-beast-devil-crn.mp3");
    mplayer->load();
    //mplayer->playpause();

    engine.addImportPath(QStringLiteral("qrc:/"));
    //engine.addImportPath(QStringLiteral("qrc:/shared"));
    engine.load(QUrl(QStringLiteral("qrc:/src/qml/main.qml")));
    //engine.load(QUrl(QStringLiteral("qrc:/src/qml/main.qml")));

    return app.exec();
}
