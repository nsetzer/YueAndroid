#-------------------------------------------------
#
# Project created by QtCreator 2017-06-05T17:57:27
#
#-------------------------------------------------


QT += core qml quick webview multimedia svg sql concurrent
android {
    QT += androidextras
}

CONFIG += c++11

TARGET = YueCommon
TEMPLATE = lib

DEFINES += YUECOMMON_LIBRARY

DEFINES += QT_DEPRECATED_WARNINGS

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#RESOURCES +=


SOURCES += \
    yue/bell/AlbumArt.cpp \
    yue/bell/database.cpp \
    yue/bell/library.cpp \
    yue/bell/LibrarySearchGrammar.cpp \
    yue/bell/LibraryTreeNode.cpp \
    yue/bell/MediaCtrlBackend.cpp \
    yue/bell/MediaCtrlBase.cpp \
    yue/bell/MediaCtrlLocal.cpp \
    yue/bell/MediaPlayerBase.cpp \
    yue/bell/MediaPlayerQt.cpp \
    yue/bell/playlist.cpp \
    yue/core/RandomArtExpressions.cpp \
    yue/core/search/grammar.cpp \
    yue/core/search/rules.cpp \
    yue/core/song.cpp \
    yue/core/util/dateutil.cpp \
    yue/device.cpp \
    yue/qtcommon/AlbumArtImage.cpp \
    yue/qtcommon/DirectoryListModel.cpp \
    yue/qtcommon/examples/BasicListModel.cpp \
    yue/qtcommon/examples/SwipeListModel.cpp \
    yue/qtcommon/LibraryTreeListModel.cpp \
    yue/qtcommon/PlaylistModel.cpp \
    yue/qtcommon/qtcommon.cpp \
    yue/qtcommon/ResourceCache.cpp \
    yue/qtcommon/SongListModel.cpp \
    yue/qtcommon/TreeListModelBase.cpp

HEADERS += \
    yue/alien/utf8/utf8.h \
    yue/alien/utf8/utf8/checked.h \
    yue/alien/utf8/utf8/core.h \
    yue/alien/utf8/utf8/unchecked.h \
    yue/bell/AlbumArt.h \
    yue/bell/database.hpp \
    yue/bell/library.hpp \
    yue/bell/LibrarySearchGrammar.hpp \
    yue/bell/LibraryTreeNode.hpp \
    yue/bell/MediaCtrlBackend.h \
    yue/bell/MediaCtrlBase.h \
    yue/bell/MediaCtrlLocal.h \
    yue/bell/MediaPlayerBase.h \
    yue/bell/MediaPlayerQt.h \
    yue/bell/playlist.hpp \
    yue/bell/SongList.h \
    yue/core/lrucache.h \
    yue/core/RandomArtExpressions.h \
    yue/core/search/grammar.hpp \
    yue/core/search/rules.hpp \
    yue/core/shuffle.hpp \
    yue/core/song.hpp \
    yue/core/util/random.h \
    yue/core/util/dateutil.h \
    yue/device.h \
    yue/global.h\
    yue/qtcommon/AlbumArtImage.h \
    yue/qtcommon/DirectoryListModel.hpp \
    yue/qtcommon/examples/BasicListModel.hpp \
    yue/qtcommon/examples/SwipeListModel.hpp \
    yue/qtcommon/LibraryTreeListModel.hpp \
    yue/qtcommon/PlaylistModel.h \
    yue/qtcommon/qtcommon.hpp \
    yue/qtcommon/ResourceCache.h \
    yue/qtcommon/SongListModel.h \
    yue/qtcommon/TreeListModelBase.hpp \
    yue/util/logging.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}
