#ifndef YUE_DEVICE_H
#define YUE_DEVICE_H

#include <QObject>
#include <QScreen>
#include <QtGui/QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QFont>
#ifdef Q_OS_ANDROID
#include <QtAndroidExtras>
#endif
#include "yue/global.h"

class YUECOMMON_EXPORT Device : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isMobile READ isMobile NOTIFY isMobileChanged)
    Q_PROPERTY(qreal dp READ dp NOTIFY dpChanged)
    Q_PROPERTY(qreal textHeight READ textHeight NOTIFY textHeightChanged)
    Q_PROPERTY(QFont font READ font NOTIFY fontChanged)

public:
    explicit Device(QObject *parent = 0);
    ~Device();

    Q_INVOKABLE qreal dp( int value ) { return m_dp*value; }

    bool isMobile() const { return m_isMobile; }
    qreal dp() const { return m_dp; }
    qreal textHeight() { return m_textHeight; }
    QFont font() { return m_font; }

signals:

    void isMobileChanged(bool arg);
    void dpChanged(qreal arg);
    void textHeightChanged(qreal textHeight);
    void fontChanged(QFont font);

public slots:

private:
    bool m_isMobile;
    QScreen *m_screen;
    int m_dpi;
    qreal m_dp;
    qreal m_textHeight;
    QFont m_font;
};

#endif // YUE_DEVICE_H
