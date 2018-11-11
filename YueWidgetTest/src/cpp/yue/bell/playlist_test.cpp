
#include "yue/bell/playlist_test.h"

void TestPlaylist::initTestCase()
{
    yue::bell::Database* db = yue::bell::Database::create();
    db->connect(":memory:");
    yue::bell::Library::create();
    yue::bell::PlaylistManager::create();
}

void TestPlaylist::cleanupTestCase()
{

}


// move index 2 to position 5, current index begins and ends at 0
void TestPlaylist::move_down_2_5_0()
{
    QSharedPointer<yue::bell::Playlist> playlist = yue::bell::PlaylistManager::instance()->open("TestPlaylist");
    QList<yue::bell::Database::uid_t> initial({'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'});
    QList<yue::bell::Database::uid_t> expected({'a', 'b', 'd', 'e', 'f', 'c', 'g', 'h'});
    playlist->set(initial);
    playlist->setCurrent(0);
    _move_impl(playlist, expected, 2, 5, 0) ;
}

// move index 2 to position 5, current index begins at 4 and ends at 3
void TestPlaylist::move_down_2_5_4()
{
    QSharedPointer<yue::bell::Playlist> playlist = yue::bell::PlaylistManager::instance()->open("TestPlaylist");
    //                                           0    1    2    3    4    5    6    7
    QList<yue::bell::Database::uid_t> initial( {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'});
    QList<yue::bell::Database::uid_t> expected({'a', 'b', 'd', 'e', 'f', 'c', 'g', 'h'});
    playlist->set(initial);
    playlist->setCurrent(4);
    _move_impl(playlist, expected, 2, 5, 3) ;
}

// move index 2 to position 5, current index begins and ends at 7
void TestPlaylist::move_down_2_5_7()
{
    QSharedPointer<yue::bell::Playlist> playlist = yue::bell::PlaylistManager::instance()->open("TestPlaylist");
    QList<yue::bell::Database::uid_t> initial( {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'});
    QList<yue::bell::Database::uid_t> expected({'a', 'b', 'd', 'e', 'f', 'c', 'g', 'h'});
    playlist->set(initial);
    playlist->setCurrent(7);
    _move_impl(playlist, expected, 2, 5, 7) ;
}

// move index 3 to position 5, current index begins at 4 and ends at 3
void TestPlaylist::move_down_3_5_4()
{
    QSharedPointer<yue::bell::Playlist> playlist = yue::bell::PlaylistManager::instance()->open("TestPlaylist");
    QList<yue::bell::Database::uid_t> initial( {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'});
    QList<yue::bell::Database::uid_t> expected({'a', 'b', 'c', 'e', 'f', 'd', 'g', 'h'});
    playlist->set(initial);
    playlist->setCurrent(4);
    _move_impl(playlist, expected, 3, 5, 3) ;
}

// move index 5 to position 2, current index begins and ends at 0
void TestPlaylist::move_up_5_2_0()
{
    QSharedPointer<yue::bell::Playlist> playlist = yue::bell::PlaylistManager::instance()->open("TestPlaylist");
    //                                           0    1    2    3    4    5    6    7
    QList<yue::bell::Database::uid_t> initial( {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'});
    QList<yue::bell::Database::uid_t> expected({'a', 'b', 'c', 'f', 'd', 'e', 'g', 'h'});
    playlist->set(initial);
    playlist->setCurrent(0);
    _move_impl(playlist, expected, 5, 2, 0) ;
}

// move index 5 to position 2, current index begins at 4 and ends at 5
void TestPlaylist::move_up_5_2_4()
{
    QSharedPointer<yue::bell::Playlist> playlist = yue::bell::PlaylistManager::instance()->open("TestPlaylist");
    //                                           0    1    2    3    4    5    6    7
    QList<yue::bell::Database::uid_t> initial( {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'});
    QList<yue::bell::Database::uid_t> expected({'a', 'b', 'c', 'f', 'd', 'e', 'g', 'h'});
    playlist->set(initial);
    playlist->setCurrent(4);
    _move_impl(playlist, expected, 5, 2, 5) ;
}

// move index 5 to position 2, current index begins and ends at 7
void TestPlaylist::move_up_5_2_7()
{
    QSharedPointer<yue::bell::Playlist> playlist = yue::bell::PlaylistManager::instance()->open("TestPlaylist");
    //                                           0    1    2    3    4    5    6    7
    QList<yue::bell::Database::uid_t> initial( {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'});
    QList<yue::bell::Database::uid_t> expected({'a', 'b', 'c', 'f', 'd', 'e', 'g', 'h'});
    playlist->set(initial);
    playlist->setCurrent(7);
    _move_impl(playlist, expected, 5, 2, 7) ;
}

// move index 5 to position 3, current index begins at 4 and ends at 5
void TestPlaylist::move_up_5_3_4()
{
    QSharedPointer<yue::bell::Playlist> playlist = yue::bell::PlaylistManager::instance()->open("TestPlaylist");
    QList<yue::bell::Database::uid_t> initial({'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'});
    QList<yue::bell::Database::uid_t> expected({'a', 'b', 'c', 'd', 'f', 'e', 'g', 'h'});
    playlist->set(initial);
    playlist->setCurrent(4);
    _move_impl(playlist, expected, 5, 3, 5) ;
}

// move index 5 to position -1, current index begins at 0 and ends at 1
void TestPlaylist::move_up_5_u_0()
{
    QSharedPointer<yue::bell::Playlist> playlist = yue::bell::PlaylistManager::instance()->open("TestPlaylist");
    QList<yue::bell::Database::uid_t> initial({'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'});
    QList<yue::bell::Database::uid_t> expected({'a', 'f', 'b', 'c', 'd', 'e', 'g', 'h'});
    playlist->set(initial);
    playlist->setCurrent(0);
    _move_impl(playlist, expected, 5, -1, 0) ;
}

/**
 * @brief TestPlaylist::_move_impl
 *
 * how the move algorithm works:
 *   0 1 2 3 4 5 6 7 // the list of indices
 *   a b c d e f g h // the list of items
 *                   // move 2 -> 5
 *   a b   d e f g h // remove 2
 *   a b d e f   g h // shift by 1
 *   a b d e f c g h // place back
 *
 *   0 1 2 3 4 5 6 7 // the list of indices
 *   a b c d e f g h // the list of items
 *                   // move 5 -> 2
 *   a b c d e   g h // remove 5
 *   a b   c d e g h // shift by 1
 *   a b f c d e g h // place back
 */
void TestPlaylist::_move_impl(playlist_t playlist, lst_t& expected, int src, int dst, int current_index)
{

    int initial_index = static_cast<int>(playlist->current().second);

    playlist->move(src, dst);

    int final_index = static_cast<int>(playlist->current().second);

    QList<yue::bell::Database::uid_t> actual;

    actual = playlist->toList();

    QCOMPARE(actual.size(), expected.size());
    bool success = true;
    for (int i=0; i < expected.size(); i++) {
        success &= actual[i] == expected[i];
    }

    // if they dont match print out the lists for debugging
    if (!success) {
        char c1[2] = {0, 0,};
        char c2[2] = {0, 0,};
        for (int i=0; i < expected.size(); i++) {
            c1[0] = static_cast<char>(actual[i]);
            c2[0] = static_cast<char>(expected[i]);
            qDebug() << i << c1 << ((actual[i]==expected[i])?'=':'!') << c2
                     << ((i==initial_index)?"*":"_")
                     << ((i==final_index)?"*":"_")
                     << ((i==current_index)?"+":"_");
        }
    }

    QCOMPARE(success, true);

    QCOMPARE(playlist->current().second, current_index);
}

QTEST_MAIN(TestPlaylist)
