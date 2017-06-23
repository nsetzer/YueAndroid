

#ifndef YUE_COMMON_YUE_UTIL_HPP
#define YUE_COMMON_YUE_UTIL_HPP

#include <QObject>
#include <QString>
#include <QElapsedTimer>
#include "yue/global.h"

namespace yue {
namespace util {

class YUECOMMON_EXPORT RaiiCallLoggerTimer : public QObject
{
    Q_OBJECT

    QString m_msg;
    QElapsedTimer m_timer;
public:
    RaiiCallLoggerTimer(QString msg)
        : QObject()
        , m_msg(msg)
        , m_timer()
    {
        m_timer.start();
    }
    ~RaiiCallLoggerTimer() {
        //QString elapsed = QString::number(m_timer.elapsed()/1000.0, 'f', 7);
        QString elapsed = QString::number(m_timer.elapsed());
        qDebug().noquote() << (m_msg + ": completed in " + elapsed + " seconds");
    }
};


} // util
} // yue

#define LOG_FUNCTION_TIME() yue::util::RaiiCallLoggerTimer(__FUNCTION__)

#endif
