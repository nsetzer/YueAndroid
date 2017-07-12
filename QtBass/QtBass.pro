#-------------------------------------------------
#
# Project created by QtCreator 2017-06-05T17:57:27
#
#-------------------------------------------------


QT += core
android {
    QT += androidextras
}

CONFIG += c++11

TARGET = QtBass
TEMPLATE = lib

DEFINES += QTBASS_LIBRARY

DEFINES += QT_DEPRECATED_WARNINGS

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#RESOURCES +=

SOURCES += QtBass.cpp


HEADERS += QtBass.hpp \
    bass.h \
    QtBass.impl.hpp

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32: LIBS += -L$$PWD/lib/win32/x86_64 -lbass
else:android: LIBS += -L$$PWD/lib/android/armeabi-v7a -lbass
