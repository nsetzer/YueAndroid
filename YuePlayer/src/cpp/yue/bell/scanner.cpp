#include "yue/bell/scanner.h"
#include "yue/core/song.hpp"

#include "taglib/taglib.h"
#include "taglib/fileref.h"
#include "taglib/id3v2tag.h"
#include "taglib/mpegfile.h"
#include "taglib/tlist.h"
#include "taglib/flacfile.h"
#include "taglib/attachedpictureframe.h"
#include "taglib/tpropertymap.h"

namespace yue {
namespace bell {

ScannerThread::ScannerThread(const QDir& root, QObject *parent)
    : QThread(parent)
    , m_dirs()
    , m_alive(true)
{
    m_dirs.append(root);
}

void ScannerThread::run()
{
    LOG_FUNCTION_TIME();

    m_db = Database::reconnect("ScannerThread");
    Database *db = m_db.data();
    m_lib = QSharedPointer<Library>(new Library(db));

    try {

        remove_missing();

        scan();
    } catch (ScannerInterrupt& ex) {
        qDebug() << "scanner interrupted" << ex.what();

    }

    m_lib.reset();
}

void ScannerThread::remove_missing()
{
    LOG_FUNCTION_TIME();

    LibrarySearchGrammar grammar;
    typedef yue::core::Song Song;
    QSqlQuery query = grammar.buildQuery(QStringList()
                          << Song::artist
                          << Song::album
                          << Song::uid
                          << Song::path,
                          "","", m_db->db());

    if (!query.exec()) {
        qWarning() << "error executing query";
        qWarning() << query.lastError();
        return;
    }

    while (query.next()) {
        check();

        QFileInfo fi(query.value(3).toString());

        if (!fi.exists()) {
            qDebug() << "Remove: " << fi;
            QMap<QString,QVariant> data;
            data[Song::artist] = query.value(0);
            data[Song::album] = query.value(1);
            data[Song::uid] = query.value(2);
            m_lib->remove(data);
        }
    }
}

void ScannerThread::scan()
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
        check();
        if (info.fileName() == "." || info.fileName() == "..") {
            continue;
        }
        if (info.isDir()) {
            m_dirs.append(QDir(info.absoluteFilePath()));
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

    TagLib::FileRef f(info.absoluteFilePath().toUtf8().constData(), true, TagLib::AudioProperties::Accurate);

    QString artist(f.tag()->artist().toCString(true));
    QString album(f.tag()->album().toCString(true));
    QString title(f.tag()->title().toCString(true));
    QString genre(f.tag()->genre().toCString(true));
    int length = f.audioProperties()->lengthInSeconds();

    artist = artist.simplified();
    album = album.simplified();
    title = title.simplified();
    genre = genre.simplified();

    if (artist.isEmpty()) {
        artist = "Unknown Artist";
    }

    if (album.isEmpty()) {
        album = "Unknown Album";
    }

    if (title.isEmpty()) {
        title = info.baseName();
    }

    QMap<QString,QVariant> data;

    data[yue::core::Song::path] = info.absoluteFilePath();
    data[yue::core::Song::artist] = artist;
    data[yue::core::Song::album] = album;
    data[yue::core::Song::title] = title;
    data[yue::core::Song::genre] = genre;
    data[yue::core::Song::length] = length;

    qDebug() << "inserting" << data[yue::core::Song::path].toString();
    Database::uid_t uid = m_lib->insert(data);
    if (uid == 0) {
        qDebug() << "failed to insert song";
        throw ScannerInterrupt();
    }

}

void ScannerThread::check()
{
    if (!m_alive) {
        throw ScannerInterrupt();
    }
    return;
}

Scanner::Scanner(QObject *parent)
    : QObject(parent)
    , m_pScannerThread(nullptr)
{

}

bool Scanner::isRunning() const
{
    return m_pScannerThread != nullptr;
}

void Scanner::start(QDir path)
{
    if (m_pScannerThread == nullptr) {
        qDebug() << "scan start" << path;
        m_pScannerThread = new ScannerThread(path, this);

        connect(m_pScannerThread, &QThread::finished,
                this, &Scanner::onScanFinished);

        connect(m_pScannerThread, &ScannerThread::update,
                this, &Scanner::onScanUpdate);

        m_pScannerThread->start();
        emit start();
    }
}

void Scanner::stop()
{
    if (m_pScannerThread) {
        m_pScannerThread->stop();
    }
}

void Scanner::onScanUpdate()
{
    emit update();
}


void Scanner::onScanFinished()
{
    qDebug() << "scan finished";
    delete m_pScannerThread;
    m_pScannerThread = nullptr;
    emit finished();
}

} // namespace bell
} // namespace yue
