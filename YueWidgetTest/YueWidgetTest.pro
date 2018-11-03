#-------------------------------------------------
#
# Project created by QtCreator 2018-11-03T14:36:29
#
#-------------------------------------------------

QT       += core gui

QT += webview multimedia remoteobjects svg sql concurrent
android{
    QT += androidextras
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = YueWidgetTest
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    src/cpp/main.cpp \
    src/cpp/mainwindow.cpp
    #src/cpp/MediaCtrlRemoteClient.cpp
    #src/cpp/MediaCtrlRemoteServer.cpp
    #src/cpp/JavaCompat.cpp

HEADERS += \
    src/cpp/mainwindow.h
    #src/cpp/MediaCtrlRemoteClient.h
    #src/cpp/MediaCtrlRemoteServer.h
    #src/cpp/JavaCompat.h

#REPC_REPLICA += src/cpp/MediaControl.rep
#REPC_SOURCE  += src/cpp/MediaControl.rep

FORMS += \
    src/cpp/mainwindow.ui

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
    ANDROID_JAVA_SOURCES.path = /src/org/github/nsetzer/example
    ANDROID_JAVA_SOURCES.files = $$files($$PWD/src/java/*.java)
    INSTALLS += ANDROID_JAVA_SOURCES
}

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../YueCommon/release/ -lYueCommon
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../YueCommon/debug/ -lYueCommon
#else:unix: LIBS += -L$$OUT_PWD/../YueCommon/ -lYueCommon

#INCLUDEPATH += $$PWD/../YueCommon
#DEPENDPATH += $$PWD/../YueCommon

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
