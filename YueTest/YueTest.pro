#-------------------------------------------------
#
# Project created by QtCreator 2018-11-03T14:36:29
#
#-------------------------------------------------

TEMPLATE = app
QT += core gui widgets testlib webview multimedia remoteobjects svg sql concurrent

CONFIG += c++11

CONFIG += testcase

INCLUDEPATH += ../YueWidgetTest/src/cpp

RESOURCES = ../YueWidgetTest/resource.qrc

SOURCES += \
    ../YueWidgetTest/src/cpp/yue/bell/AlbumArt.cpp \
    ../YueWidgetTest/src/cpp/yue/bell/database.cpp \
    ../YueWidgetTest/src/cpp/yue/bell/library.cpp \
    ../YueWidgetTest/src/cpp/yue/bell/LibrarySearchGrammar.cpp \
    ../YueWidgetTest/src/cpp/yue/bell/LibraryTreeNode.cpp \
    ../YueWidgetTest/src/cpp/yue/bell/MediaCtrlBackend.cpp \
    ../YueWidgetTest/src/cpp/yue/bell/MediaCtrlBase.cpp \
    ../YueWidgetTest/src/cpp/yue/bell/MediaCtrlLocal.cpp \
    ../YueWidgetTest/src/cpp/yue/bell/MediaPlayerBase.cpp \
    ../YueWidgetTest/src/cpp/yue/bell/MediaPlayerQt.cpp \
    ../YueWidgetTest/src/cpp/yue/bell/playlist.cpp \
    ../YueWidgetTest/src/cpp/yue/bell/settings.cpp \
    ../YueWidgetTest/src/cpp/yue/core/RandomArtExpressions.cpp \
    ../YueWidgetTest/src/cpp/yue/core/search/grammar.cpp \
    ../YueWidgetTest/src/cpp/yue/core/search/rules.cpp \
    ../YueWidgetTest/src/cpp/yue/core/song.cpp \
    ../YueWidgetTest/src/cpp/yue/core/util/dateutil.cpp \
    ../YueWidgetTest/src/cpp/yue/qtcommon/DirectoryListModel.cpp \
    ../YueWidgetTest/src/cpp/yue/qtcommon/examples/BasicListModel.cpp \
    ../YueWidgetTest/src/cpp/yue/qtcommon/examples/SwipeListModel.cpp \
    ../YueWidgetTest/src/cpp/yue/qtcommon/LibraryTreeListModel.cpp \
    ../YueWidgetTest/src/cpp/yue/qtcommon/PlaylistModel.cpp \
    ../YueWidgetTest/src/cpp/yue/qtcommon/ResourceCache.cpp \
    ../YueWidgetTest/src/cpp/yue/qtcommon/SongListModel.cpp \
    ../YueWidgetTest/src/cpp/yue/qtcommon/TreeListModelBase.cpp \
    ../YueWidgetTest/src/cpp/yue/qtcommon/toolbar.cpp

HEADERS += \
    ../YueWidgetTest/src/cpp/yue/alien/utf8/utf8.h \
    ../YueWidgetTest/src/cpp/yue/alien/utf8/utf8/checked.h \
    ../YueWidgetTest/src/cpp/yue/alien/utf8/utf8/core.h \
    ../YueWidgetTest/src/cpp/yue/alien/utf8/utf8/unchecked.h \
    ../YueWidgetTest/src/cpp/yue/bell/AlbumArt.h \
    ../YueWidgetTest/src/cpp/yue/bell/database.hpp \
    ../YueWidgetTest/src/cpp/yue/bell/library.hpp \
    ../YueWidgetTest/src/cpp/yue/bell/LibrarySearchGrammar.hpp \
    ../YueWidgetTest/src/cpp/yue/bell/LibraryTreeNode.hpp \
    ../YueWidgetTest/src/cpp/yue/bell/MediaCtrlBackend.h \
    ../YueWidgetTest/src/cpp/yue/bell/MediaCtrlBase.h \
    ../YueWidgetTest/src/cpp/yue/bell/MediaCtrlLocal.h \
    ../YueWidgetTest/src/cpp/yue/bell/MediaPlayerBase.h \
    ../YueWidgetTest/src/cpp/yue/bell/MediaPlayerQt.h \
    ../YueWidgetTest/src/cpp/yue/bell/playlist.hpp \
    ../YueWidgetTest/src/cpp/yue/bell/SongList.h \
    ../YueWidgetTest/src/cpp/yue/bell/settings.h \
    ../YueWidgetTest/src/cpp/yue/core/lrucache.h \
    ../YueWidgetTest/src/cpp/yue/core/RandomArtExpressions.h \
    ../YueWidgetTest/src/cpp/yue/core/search/grammar.hpp \
    ../YueWidgetTest/src/cpp/yue/core/search/rules.hpp \
    ../YueWidgetTest/src/cpp/yue/core/shuffle.hpp \
    ../YueWidgetTest/src/cpp/yue/core/song.hpp \
    ../YueWidgetTest/src/cpp/yue/core/util/dateutil.h \
    ../YueWidgetTest/src/cpp/yue/core/util/random.h \
    ../YueWidgetTest/src/cpp/yue/util/logging.h \
    ../YueWidgetTest/src/cpp/yue/global.h \
    ../YueWidgetTest/src/cpp/yue/qtcommon/DirectoryListModel.h \
    ../YueWidgetTest/src/cpp/yue/qtcommon/examples/BasicListModel.hpp \
    ../YueWidgetTest/src/cpp/yue/qtcommon/examples/SwipeListModel.hpp \
    ../YueWidgetTest/src/cpp/yue/qtcommon/LibraryTreeListModel.h \
    ../YueWidgetTest/src/cpp/yue/qtcommon/PlaylistModel.h \
    ../YueWidgetTest/src/cpp/yue/qtcommon/ResourceCache.h \
    ../YueWidgetTest/src/cpp/yue/qtcommon/SongListModel.h \
    ../YueWidgetTest/src/cpp/yue/qtcommon/TreeListModelBase.h \
    ../YueWidgetTest/src/cpp/yue/qtcommon/toolbar.h \
    ../YueWidgetTest/src/cpp/yue/qtcommon/iconbutton.h \
    ../YueWidgetTest/src/cpp/yue/qtcommon/svg.h

SOURCES += \
    ../YueWidgetTest/src/cpp/yue/bell/playlist_test.cpp

HEADERS += \
    ../YueWidgetTest/src/cpp/yue/bell/playlist_test.h
