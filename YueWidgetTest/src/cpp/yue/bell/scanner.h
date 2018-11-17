#ifndef YUE_BELL_SCANNER_H
#define YUE_BELL_SCANNER_H

#include <QObject>
#include <QThread>
#include <QDir>
#include <QDebug>
#include "yue/global.h"
#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"

namespace yue {
namespace bell {

class ScannerInterrupt : public std::exception {

};

class ScannerThread : public QThread
{
    Q_OBJECT

public:
    ScannerThread(const QDir& root, QObject *parent = nullptr);

    void run() override;


signals:

private:
    void remove_missing();
    void scan();
    void scan_dir(const QDir& dir);
    void scan_file(const QFileInfo& path);
    void check();

    QList<QDir> m_dirs; // list of dirs to scan
    QSharedPointer<Database> m_db;
    QSharedPointer<Library> m_lib;
    bool m_alive;

};

class Scanner : public QObject
{
    Q_OBJECT
public:
    explicit Scanner(QObject *parent = nullptr);

    bool isRunning() const;

signals:

    void start();
    void finished();

public slots:

    void start(QDir path);
    void stop();

private:
    ScannerThread *m_pScannerThread;
};

} // namespace bell
} // namespace yue


#endif // SCANNER_H
