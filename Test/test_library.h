#ifndef TEST_LIBRARY_H
#define TEST_LIBRARY_H

#include <QObject>
#include <QString>
#include <QList>

#include "util.h"

#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
#include "yue/bell/playlist.hpp"

class Task : public QObject
{
    Q_OBJECT
public:
    Task(QObject *parent = 0) : QObject(parent) {}

public slots:
    void run();

signals:
    void finished();
};

int test_library( void );


#endif // TEST_PLAYLIST_H
