#include "yue/bell/scanner.h"
#include "yue/core/song.hpp"

#include "taglib/taglib.h"
#include "taglib/fileref.h"
#include "taglib/id3v2tag.h"
#include "taglib/mpegfile.h"
#include "taglib/tlist.h"
#include "taglib/flacfile.h"
#include "taglib/attachedpictureframe.h"



//#include "id3v2lib/id3v2lib.h"

namespace yue {
namespace bell {

ScannerThread::ScannerThread(const QDir& root, QObject *parent)
    : QThread(parent)
    , m_dirs()
{
    m_dirs.append(root);
}

void ScannerThread::run()
{
    m_db = Database::reconnect();
    Database *db = m_db.data();
    m_lib = QSharedPointer<Library>(new Library(db));

    while (!m_dirs.isEmpty()) {
        QDir dir = m_dirs.back();
        m_dirs.pop_back();

        scan_dir(dir);
    }
}

void ScannerThread::scan_dir(const QDir& dir)
{
    qDebug() << "Scanning Directory: " << dir.absolutePath();

    for(const QFileInfo& info : dir.entryInfoList(QDir::Files | QDir::Dirs)) {
        if (info.isDir()) {
            m_dirs.append(QDir(info.absolutePath()));
        } else {
            scan_file(info);
        }
    }
}

void ScannerThread::scan_file(const QFileInfo& info)
{
    if (!info.absoluteFilePath().toLower().endsWith(".mp3")) {
        return;
    }

    if (m_lib->exists(info.absoluteFilePath())) {
        qDebug() << info.absoluteFilePath() << " rejecting path. already exists.";
        return;
    }

    TagLib::FileRef f(info.absoluteFilePath().toUtf8().constData());

    QString artist(f.tag()->artist().toCString(true));
    QString album(f.tag()->album().toCString(true));
    QString title(f.tag()->title().toCString(true));
    QString genre(f.tag()->genre().toCString(true));

    QMap<QString,QVariant> data;

    data[yue::core::Song::path] = info.absoluteFilePath();
    data[yue::core::Song::artist] = artist;
    data[yue::core::Song::album] = album;
    data[yue::core::Song::title] = title;
    data[yue::core::Song::genre] = genre;

    m_lib->insert(data);

    //ID3v2_tag* tag = load_tag(info.absoluteFilePath().toUtf8().constData());

}

Scanner::Scanner(QObject *parent) : QObject(parent)
{

}

void Scanner::startScan(QDir path)
{

}

} // namespace bell
} // namespace yue
