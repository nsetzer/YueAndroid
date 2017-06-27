
#include "yue/device.h"
#include <QFontMetricsF>

#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#endif

#include <QStandardPaths>
#include <QDir>

namespace yue {
namespace qtcommon {

Device* Device::m_instance=nullptr;


QString Device::DIRECTORY_DCIM = "DIRECTORY_DCIM";
QString Device::DIRECTORY_DOCUMENTS = "DIRECTORY_DOCUMENTS";
QString Device::DIRECTORY_MOVIES = "DIRECTORY_MOVIES";
QString Device::DIRECTORY_MUSIC = "DIRECTORY_MUSIC";
QString Device::DIRECTORY_PICTURES = "DIRECTORY_PICTURES";

Device::Device(QObject *parent) : QObject(parent)
{
    m_screen = qApp->primaryScreen();
    m_dpi = m_screen->physicalDotsPerInch() * m_screen->devicePixelRatio();
    m_isMobile = false;
#if defined(Q_OS_IOS)
    m_dpi = m_screen->physicalDotsPerInch();
    m_isMobile = true;
#elif defined(Q_OS_ANDROID)
    QAndroidJniEnvironment env;
    QAndroidJniObject activity = QtAndroid::androidActivity();
    QAndroidJniObject resources = activity.callObjectMethod("getResources", "()Landroid/content/res/Resources;");
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();

        //return EXIT_FAILURE;
    }

    QAndroidJniObject displayMetrics = resources.callObjectMethod("getDisplayMetrics", "()Landroid/util/DisplayMetrics;");
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();

        //return EXIT_FAILURE;
    }
    m_dpi = displayMetrics.getField<int>("densityDpi");
    m_isMobile = true;

#else
    // standard dpi
    m_dpi = m_screen->logicalDotsPerInch() * m_screen->devicePixelRatio();
#endif


    m_dp = m_dpi / 160.f;
    qDebug() << "*** dp:"<<m_dp<<" dpi:"<<m_dpi;
    emit dpChanged(m_dp);
    emit isMobileChanged(m_isMobile);

    m_font = QFont();
    QFontMetricsF fm = QFontMetricsF(m_font);
    m_textHeight = fm.height();
    emit textHeightChanged(m_textHeight);
    emit fontChanged(m_font);

}

Device::~Device()
{
}



QString Device::getDirectory(const QString &type)
{
    QString res;

#ifdef Q_OS_ANDROID
    QAndroidJniObject jniType = QAndroidJniObject::getStaticObjectField<jstring>("android/os/Environment", type.toUtf8().constData());

    qDebug() << "environment: jni" << jniType.isValid();
    QAndroidJniObject file = QAndroidJniObject::callStaticObjectMethod("android/os/Environment",
                                                                       "getExternalStoragePublicDirectory",
                                                                       "(Ljava/lang/String;)Ljava/io/File;",
                                                                       jniType.object());
    qDebug() << "environment: file" << file.isValid();
    QAndroidJniObject absolutePath = file.callObjectMethod("getAbsolutePath","()Ljava/lang/String;");
    qDebug() << "environment: absolutePath" << absolutePath.isValid();
    res = absolutePath.toString();

#else
    if (type == DIRECTORY_DCIM) {
        res = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)[0];
    } else {
        res = QDir::currentPath();
    }
#endif
    return res;
}



} // qtcommon
} // yue
