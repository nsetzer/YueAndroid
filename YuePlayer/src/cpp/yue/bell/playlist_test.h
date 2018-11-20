#ifndef YUE_BELL_PLAYLIST_TEST_H
#define YUE_BELL_PLAYLIST_TEST_H

#include <QtTest/QtTest>

#include <QSharedPointer>

#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
#include "yue/bell/playlist.hpp"

class TestPlaylist: public QObject
{
    Q_OBJECT
    typedef yue::bell::Database::uid_t uid_t;
    typedef QList<uid_t> lst_t;
    typedef QSharedPointer<yue::bell::Playlist> playlist_t;
private slots:
    void initTestCase();
    void move_down_2_5_0();
    void move_down_2_5_4();
    void move_down_2_5_7();
    void move_down_3_5_4();
    void move_up_5_2_0();
    void move_up_5_2_4();
    void move_up_5_2_7();
    void move_up_5_3_4();
    void move_up_5_u_0();
    void cleanupTestCase();

private:
    void _move_impl(playlist_t playlist, lst_t& expected, int src, int dst, int current_index);
};

#endif
