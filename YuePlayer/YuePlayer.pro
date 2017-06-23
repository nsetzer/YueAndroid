TEMPLATE = app
QT += core qml quick webview multimedia remoteobjects svg sql
android{
    QT += androidextras
}

CONFIG += c++11

SOURCES += \
    src/cpp/main.cpp \
    src/cpp/MediaCtrlRemoteClient.cpp \
    src/cpp/MediaCtrlRemoteServer.cpp

HEADERS += \
    src/cpp/MediaCtrlRemoteClient.h \
    src/cpp/MediaCtrlRemoteServer.h

REPC_REPLICA += src/cpp/MediaControl.rep
REPC_SOURCE  += src/cpp/MediaControl.rep

RESOURCES += qml.qrc \
    ../SharedQml/shared.qrc

# Default rules for deployment.
#include(deployment.pri)

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../YueCommon/ -lYueCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../YueCommon/ -lYueCommond
else:unix: LIBS += -L$$OUT_PWD/../YueCommon/ -lYueCommon

INCLUDEPATH += $$PWD/../YueCommon
DEPENDPATH += $$PWD/../YueCommon
