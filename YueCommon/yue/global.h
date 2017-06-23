#ifndef YUE_COMMON_GLOBAL_H
#define YUE_COMMON_GLOBAL_H


#include <QtCore/qglobal.h>
#include <QObject>
#include <QString>
#include <QDebug>
#include <QTime>
#include <QtGlobal>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QMutexLocker>

#if defined(YUECOMMON_LIBRARY)
#  define YUECOMMON_EXPORT Q_DECL_EXPORT
#else
#  define YUECOMMON_EXPORT Q_DECL_IMPORT
#endif

#endif // YUE_COMMON_GLOBAL_H

#include "yue/util/logging.hpp"
