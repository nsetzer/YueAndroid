
QT += core qml quick webview multimedia svg sql

android{
    QT += androidextras
}

CONFIG += c++11

SOURCES += src/main.cpp

HEADERS +=

RESOURCES += src/qml/qml.qrc \
    ../SharedQml/shared.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
include(deployment.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../YueCommon/release/ -lYueCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../YueCommon/debug/ -lYueCommon
else:unix: LIBS += -L$$OUT_PWD/../YueCommon/ -lYueCommon

INCLUDEPATH += $$PWD/../YueCommon
DEPENDPATH += $$PWD/../YueCommon

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

//ANDROID_EXTRA_LIBS=C:\Qt\5.9\android_armv7\lib\libQt5Sql.so
