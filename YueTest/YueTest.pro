#-------------------------------------------------
#
# Project created by QtCreator 2018-11-03T14:36:29
#
#-------------------------------------------------

TEMPLATE = app
QT += core gui widgets testlib webview multimedia remoteobjects svg sql concurrent

CONFIG += c++11

CONFIG += testcase

INCLUDEPATH += ../YuePlayer/src/cpp

RESOURCES = ../YuePlayer/resource.qrc

SOURCES += \
    ../YuePlayer/src/cpp/yue/bell/AlbumArt.cpp \
    ../YuePlayer/src/cpp/yue/bell/database.cpp \
    ../YuePlayer/src/cpp/yue/bell/library.cpp \
    ../YuePlayer/src/cpp/yue/bell/LibrarySearchGrammar.cpp \
    ../YuePlayer/src/cpp/yue/bell/LibraryTreeNode.cpp \
    ../YuePlayer/src/cpp/yue/bell/MediaCtrlBackend.cpp \
    ../YuePlayer/src/cpp/yue/bell/MediaCtrlBase.cpp \
    ../YuePlayer/src/cpp/yue/bell/MediaCtrlLocal.cpp \
    ../YuePlayer/src/cpp/yue/bell/MediaPlayerBase.cpp \
    ../YuePlayer/src/cpp/yue/bell/MediaPlayerQt.cpp \
    ../YuePlayer/src/cpp/yue/bell/playlist.cpp \
    ../YuePlayer/src/cpp/yue/bell/settings.cpp \
    ../YuePlayer/src/cpp/yue/bell/scanner.cpp \
    ../YuePlayer/src/cpp/yue/bell/remote/client.cpp \
    ../YuePlayer/src/cpp/yue/bell/remote/client_test.cpp \
    ../YuePlayer/src/cpp/yue/core/RandomArtExpressions.cpp \
    ../YuePlayer/src/cpp/yue/core/search/grammar.cpp \
    ../YuePlayer/src/cpp/yue/core/search/rules.cpp \
    ../YuePlayer/src/cpp/yue/core/song.cpp \
    ../YuePlayer/src/cpp/yue/core/util/dateutil.cpp \
    ../YuePlayer/src/cpp/yue/qtcommon/DirectoryListModel.cpp \
    ../YuePlayer/src/cpp/yue/qtcommon/examples/BasicListModel.cpp \
    ../YuePlayer/src/cpp/yue/qtcommon/examples/SwipeListModel.cpp \
    ../YuePlayer/src/cpp/yue/qtcommon/LibraryTreeListModel.cpp \
    ../YuePlayer/src/cpp/yue/qtcommon/PlaylistModel.cpp \
    ../YuePlayer/src/cpp/yue/qtcommon/ResourceCache.cpp \
    ../YuePlayer/src/cpp/yue/qtcommon/SongListModel.cpp \
    ../YuePlayer/src/cpp/yue/qtcommon/TreeListModelBase.cpp \
    ../YuePlayer/src/cpp/yue/qtcommon/toolbar.cpp

HEADERS += \
    ../YuePlayer/src/cpp/yue/alien/utf8/utf8.h \
    ../YuePlayer/src/cpp/yue/alien/utf8/utf8/checked.h \
    ../YuePlayer/src/cpp/yue/alien/utf8/utf8/core.h \
    ../YuePlayer/src/cpp/yue/alien/utf8/utf8/unchecked.h \
    ../YuePlayer/src/cpp/yue/bell/AlbumArt.h \
    ../YuePlayer/src/cpp/yue/bell/database.hpp \
    ../YuePlayer/src/cpp/yue/bell/library.hpp \
    ../YuePlayer/src/cpp/yue/bell/LibrarySearchGrammar.hpp \
    ../YuePlayer/src/cpp/yue/bell/LibraryTreeNode.hpp \
    ../YuePlayer/src/cpp/yue/bell/MediaCtrlBackend.h \
    ../YuePlayer/src/cpp/yue/bell/MediaCtrlBase.h \
    ../YuePlayer/src/cpp/yue/bell/MediaCtrlLocal.h \
    ../YuePlayer/src/cpp/yue/bell/MediaPlayerBase.h \
    ../YuePlayer/src/cpp/yue/bell/MediaPlayerQt.h \
    ../YuePlayer/src/cpp/yue/bell/playlist.hpp \
    ../YuePlayer/src/cpp/yue/bell/SongList.h \
    ../YuePlayer/src/cpp/yue/bell/settings.h \
    ../YuePlayer/src/cpp/yue/bell/scanner.h \
    ../YuePlayer/src/cpp/yue/bell/remote/client.hpp \
    ../YuePlayer/src/cpp/yue/bell/remote/client_test.hpp \
    ../YuePlayer/src/cpp/yue/core/lrucache.h \
    ../YuePlayer/src/cpp/yue/core/RandomArtExpressions.h \
    ../YuePlayer/src/cpp/yue/core/search/grammar.hpp \
    ../YuePlayer/src/cpp/yue/core/search/rules.hpp \
    ../YuePlayer/src/cpp/yue/core/shuffle.hpp \
    ../YuePlayer/src/cpp/yue/core/song.hpp \
    ../YuePlayer/src/cpp/yue/core/util/dateutil.h \
    ../YuePlayer/src/cpp/yue/core/util/random.h \
    ../YuePlayer/src/cpp/yue/util/logging.h \
    ../YuePlayer/src/cpp/yue/global.h \
    ../YuePlayer/src/cpp/yue/qtcommon/DirectoryListModel.h \
    ../YuePlayer/src/cpp/yue/qtcommon/examples/BasicListModel.hpp \
    ../YuePlayer/src/cpp/yue/qtcommon/examples/SwipeListModel.hpp \
    ../YuePlayer/src/cpp/yue/qtcommon/LibraryTreeListModel.h \
    ../YuePlayer/src/cpp/yue/qtcommon/PlaylistModel.h \
    ../YuePlayer/src/cpp/yue/qtcommon/ResourceCache.h \
    ../YuePlayer/src/cpp/yue/qtcommon/SongListModel.h \
    ../YuePlayer/src/cpp/yue/qtcommon/TreeListModelBase.h \
    ../YuePlayer/src/cpp/yue/qtcommon/toolbar.h \
    ../YuePlayer/src/cpp/yue/qtcommon/iconbutton.h \
    ../YuePlayer/src/cpp/yue/qtcommon/svg.h

SOURCES += \
    ../YuePlayer/src/cpp/yue/bell/playlist_test.cpp

HEADERS += \
    ../YuePlayer/src/cpp/yue/bell/playlist_test.h


#-------------------------------------------------
# configure TAGLIB
INCLUDEPATH += $$PWD/../TagLib/include
DEFINES += TAGLIB_STATIC
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../TagLib/release/ -ltag
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../TagLib/debug/ -ltag
else:unix: LIBS += -L$$OUT_PWD/../TagLib/ -ltag
