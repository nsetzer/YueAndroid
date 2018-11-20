
#include <QStandardPaths>
#include <QDebug>

#include "yue/bell/settings.h"

namespace yue {
namespace bell {

Settings* Settings::m_instance = nullptr;

Settings::Settings()
    : QObject()
    , settings(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/config.ini", QSettings::IniFormat)
{
    qDebug() << "saving settings to: " << settings.fileName();
}

Settings::~Settings()
{

}

QList<QPair<QString, QString>> Settings::dynamicLists()
{
    QList<QPair<QString, QString>> lst;

    int size = settings.beginReadArray("playlist/dynamic");
    for (int i=0; i < size; i++) {
        settings.setArrayIndex(i);
        QString name = settings.value("name").toString();
        QString query = settings.value("query").toString();
        lst.append(QPair<QString, QString>(name, query));
    }
    settings.endArray();

    return lst;
}

void Settings::setDynamicLists(QList<QPair<QString, QString>> lst)
{
    settings.beginWriteArray("playlist/dynamic");
    int i=0;
    for (const QPair<QString, QString>& item : lst) {
        settings.setArrayIndex(i);
        settings.setValue("name", item.first);
        settings.setValue("query", item.second);
        ++i;
    }
    settings.endArray();

    qDebug() << "dynamic list updated (" << lst.size() << ")";
}


} // namespace bell
} // namespace yue

