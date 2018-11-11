#include "yue/bell/scanner.h"

#include "taglib/taglib.h"
#include "taglib/fileref.h"
#include "taglib/id3v2tag.h"
#include "taglib/mpegfile.h"
#include "taglib/tlist.h"
#include "taglib/flacfile.h"
#include "taglib/attachedpictureframe.h"

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



    TagLib::FileRef f(info.absoluteFilePath().toUtf8().constData());

    QString artist(f.tag()->artist().toCString(true));
    QString album(f.tag()->album().toCString(true));
    QString title(f.tag()->title().toCString(true));

    //std::cout << f.tag()->genre() << std::endl;

}

Scanner::Scanner(QObject *parent) : QObject(parent)
{

}

void Scanner::startScan(QDir path)
{

}

} // namespace bell
} // namespace yue
