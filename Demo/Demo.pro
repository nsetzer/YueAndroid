TEMPLATE = app

QT += core gui qml quick webview multimedia svg sql
android{
    QT += androidextras
}
CONFIG += c++11


SOURCES += \
    src/cpp/main.cpp

RESOURCES += qml.qrc \
    ../SharedQml/shared.qrc

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/gradle.properties \
    android/local.properties

android {
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    #ANDROID_JAVA_SOURCES
    #INSTALLS += ANDROID_JAVA_SOURCES
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../YueCommon/release/ -lYueCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../YueCommon/debug/ -lYueCommon
else:unix: LIBS += -L$$OUT_PWD/../YueCommon/ -lYueCommon

INCLUDEPATH += $$PWD/../YueCommon
DEPENDPATH += $$PWD/../YueCommon

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

