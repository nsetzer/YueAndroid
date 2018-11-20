#-------------------------------------------------
#
# Project created by QtCreator 2018-11-03T14:36:29
#
#-------------------------------------------------

# MyCustomAppService
# MyCustomAppActivity
TEMPLATE = app
QT += core gui widgets webview multimedia remoteobjects svg sql concurrent

android{
    QT += androidextras
}

CONFIG += c++11

INCLUDEPATH += src/cpp

RESOURCES = resource.qrc

SOURCES += \
    src/cpp/main.cpp \
    src/cpp/rpc/JavaCompat.cpp \
    src/cpp/rpc/MediaCtrlRemoteClient.cpp \
    src/cpp/rpc/MediaCtrlRemoteServer.cpp \
    src/cpp/ui/mainwindow.cpp \
    src/cpp/ui/page_dynamic.cpp \
    src/cpp/ui/page_explorer.cpp \
    src/cpp/ui/page_library.cpp \
    src/cpp/ui/page_player.cpp \
    src/cpp/ui/page_queue.cpp \
    src/cpp/ui/page_settings.cpp \
    src/cpp/yue/bell/AlbumArt.cpp \
    src/cpp/yue/bell/database.cpp \
    src/cpp/yue/bell/library.cpp \
    src/cpp/yue/bell/LibrarySearchGrammar.cpp \
    src/cpp/yue/bell/LibraryTreeNode.cpp \
    src/cpp/yue/bell/MediaCtrlBackend.cpp \
    src/cpp/yue/bell/MediaCtrlBase.cpp \
    src/cpp/yue/bell/MediaCtrlLocal.cpp \
    src/cpp/yue/bell/MediaPlayerBase.cpp \
    src/cpp/yue/bell/MediaPlayerQt.cpp \
    src/cpp/yue/bell/playlist.cpp \
    src/cpp/yue/bell/scanner.cpp \
    src/cpp/yue/bell/settings.cpp \
    src/cpp/yue/core/RandomArtExpressions.cpp \
    src/cpp/yue/core/search/grammar.cpp \
    src/cpp/yue/core/search/rules.cpp \
    src/cpp/yue/core/song.cpp \
    src/cpp/yue/core/util/dateutil.cpp \
    src/cpp/yue/qtcommon/DirectoryListModel.cpp \
    src/cpp/yue/qtcommon/examples/BasicListModel.cpp \
    src/cpp/yue/qtcommon/examples/SwipeListModel.cpp \
    src/cpp/yue/qtcommon/LibraryTreeListModel.cpp \
    src/cpp/yue/qtcommon/PlaylistModel.cpp \
    src/cpp/yue/qtcommon/ResourceCache.cpp \
    src/cpp/yue/qtcommon/SongListModel.cpp \
    src/cpp/yue/qtcommon/toolbar.cpp \
    src/cpp/yue/qtcommon/TreeListModelBase.cpp \
    src/cpp/yue/qtcommon/gesture.cpp \
    src/cpp/yue/qtcommon/mediaprogressbar.cpp

# src/cpp/yue/qtcommon/AlbumArtImage.cpp

HEADERS += \
    src/cpp/rpc/JavaCompat.h \
    src/cpp/rpc/MediaCtrlRemoteClient.h \
    src/cpp/rpc/MediaCtrlRemoteServer.h \
    src/cpp/ui/mainwindow.h \
    src/cpp/ui/page_dynamic.h \
    src/cpp/ui/page_explorer.h \
    src/cpp/ui/page_library.h \
    src/cpp/ui/page_player.h \
    src/cpp/ui/page_queue.h \
    src/cpp/ui/page_settings.h \
    src/cpp/yue/alien/utf8/utf8.h \
    src/cpp/yue/alien/utf8/utf8/checked.h \
    src/cpp/yue/alien/utf8/utf8/core.h \
    src/cpp/yue/alien/utf8/utf8/unchecked.h \
    src/cpp/yue/bell/AlbumArt.h \
    src/cpp/yue/bell/database.hpp \
    src/cpp/yue/bell/library.hpp \
    src/cpp/yue/bell/LibrarySearchGrammar.hpp \
    src/cpp/yue/bell/LibraryTreeNode.hpp \
    src/cpp/yue/bell/MediaCtrlBackend.h \
    src/cpp/yue/bell/MediaCtrlBase.h \
    src/cpp/yue/bell/MediaCtrlLocal.h \
    src/cpp/yue/bell/MediaPlayerBase.h \
    src/cpp/yue/bell/MediaPlayerQt.h \
    src/cpp/yue/bell/playlist.hpp \
    src/cpp/yue/bell/scanner.h \
    src/cpp/yue/bell/settings.h \
    src/cpp/yue/bell/SongList.h \
    src/cpp/yue/core/lrucache.h \
    src/cpp/yue/core/RandomArtExpressions.h \
    src/cpp/yue/core/search/grammar.hpp \
    src/cpp/yue/core/search/rules.hpp \
    src/cpp/yue/core/shuffle.hpp \
    src/cpp/yue/core/song.hpp \
    src/cpp/yue/core/util/dateutil.h \
    src/cpp/yue/core/util/random.h \
    src/cpp/yue/global.h \
    src/cpp/yue/qtcommon/DirectoryListModel.h \
    src/cpp/yue/qtcommon/examples/BasicListModel.hpp \
    src/cpp/yue/qtcommon/examples/SwipeListModel.hpp \
    src/cpp/yue/qtcommon/iconbutton.h \
    src/cpp/yue/qtcommon/LibraryTreeListModel.h \
    src/cpp/yue/qtcommon/PlaylistModel.h \
    src/cpp/yue/qtcommon/ResourceCache.h \
    src/cpp/yue/qtcommon/SongListModel.h \
    src/cpp/yue/qtcommon/svg.h \
    src/cpp/yue/qtcommon/toolbar.h \
    src/cpp/yue/qtcommon/TreeListModelBase.h \
    src/cpp/yue/util/logging.h \
    src/cpp/yue/qtcommon/gesture.h \
    src/cpp/yue/qtcommon/mediaprogressbar.h

# src/cpp/yue/qtcommon/AlbumArtImage.h


#FORMS += \
#    src/cpp/mainwindow.ui

REPC_REPLICA += src/cpp/rpc/MediaControl.rep
REPC_SOURCE  += src/cpp/rpc/MediaControl.rep

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/gradle.properties \
    android/local.properties \
    android/res/values/apptheme.xml \
    android/res/drawable/splash.xml \
    android/res/drawable/icon.png

android {
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    ANDROID_JAVA_SOURCES.path = /src/org/github/nsetzer/yueapp2
    ANDROID_JAVA_SOURCES.files = $$files($$PWD/src/java/org/github/nsetzer/yueapp2/*.java)
    INSTALLS += ANDROID_JAVA_SOURCES
}

#-------------------------------------------------
# configure id3v2lib

#SOURCES += \
#    src/cpp/id3v2lib/frame.c \
#    src/cpp/id3v2lib/header.c \
#    src/cpp/id3v2lib/id3v2lib.c \
#    src/cpp/id3v2lib/types.c \
#    src/cpp/id3v2lib/utils.c
#
#HEADERS += \
#    src/cpp/id3v2lib/constants.h \
#    src/cpp/id3v2lib/frame.h \
#    src/cpp/id3v2lib/header.h \
#    src/cpp/id3v2lib/id3v2lib.h \
#    src/cpp/id3v2lib/types.h \
#    src/cpp/id3v2lib/utils.h

#-------------------------------------------------
# configure TAGLIB
INCLUDEPATH += $$PWD/../TagLib/include
DEFINES += TAGLIB_STATIC
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../TagLib/release/ -ltag
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../TagLib/debug/ -ltag
else:unix: LIBS += -L$$OUT_PWD/../TagLib/ -ltag
