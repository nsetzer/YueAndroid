#ifndef YUE_BELL_SCANNER_H
#define YUE_BELL_SCANNER_H

#include <QObject>
#include <QThread>
#include <QDir>
#include <QDebug>

namespace yue {
namespace bell {

class ScannerThread : public QThread
{
    Q_OBJECT

    ScannerThread(const QDir& root, QObject *parent = nullptr);

    void run() override;

    void scan_dir(const QDir& dir);

    void scan_file(const QFileInfo& path);
signals:

private:
    QList<QDir> m_dirs; // list of dirs to scan


};

class Scanner : public QObject
{
    Q_OBJECT
public:
    explicit Scanner(QObject *parent = nullptr);

    bool running() const;

signals:

    void start();
    void finished();

public slots:

    void startScan(QDir path);
};

} // namespace bell
} // namespace yue


#endif // SCANNER_H
