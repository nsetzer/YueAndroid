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

SOURCES += \
    src/cpp/main.cpp \
    src/cpp/mainwindow.cpp \
    src/cpp/MediaCtrlRemoteClient.cpp \
    src/cpp/MediaCtrlRemoteServer.cpp \
    src/cpp/JavaCompat.cpp

HEADERS += \
    src/cpp/mainwindow.h \
    src/cpp/MediaCtrlRemoteClient.h \
    src/cpp/MediaCtrlRemoteServer.h \
    src/cpp/JavaCompat.h

FORMS += \
    src/cpp/mainwindow.ui

REPC_REPLICA += src/cpp/MediaControl.rep
REPC_SOURCE  += src/cpp/MediaControl.rep

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
