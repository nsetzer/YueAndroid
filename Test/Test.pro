QT += core qml quick webview multimedia svg sql
QT -= gui

CONFIG += c++11

TARGET = Test
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    test_playlist.cpp \
    test_library.cpp \
    util.cpp

HEADERS += \
    test_playlist.h \
    test_library.h \
    util.h


# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../YueCommon/release/ -lYueCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../YueCommon/debug/ -lYueCommon
else:unix: LIBS += -L$$OUT_PWD/../YueCommon/ -lYueCommon

INCLUDEPATH += $$PWD/../YueCommon
DEPENDPATH += $$PWD/../YueCommon

