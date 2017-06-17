#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QtAndroid>
#endif

#include "yue/qtcommon/qtcommon.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    yue::qtcommon::registerQmlTypes();

    QQmlApplicationEngine engine;

    if (QCoreApplication::arguments().count() > 1){
        qDebug() << "I am the service";
        qDebug("arguments: %s", qPrintable(QCoreApplication::arguments()[0]));
    }
    else {
        qDebug() << "I am the app";
        engine.load(QUrl(QLatin1String("qrc:/main.qml")));
        if (engine.rootObjects().isEmpty())
            return -1;
    }
    return app.exec();
}
