#ifndef JAVACOMPAT_H
#define JAVACOMPAT_H

#include <QDebug>

#ifdef Q_OS_ANDROID
#include <jni.h>
#include <QAndroidJniEnvironment>
#endif


class JavaCompat
{
public:
    JavaCompat() {}
    ~JavaCompat() = default;

    static int RegisterNatives();
};

#endif // JAVACOMPAT_H
