#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QList>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>

template<typename T>
bool check_value(QSqlQuery& query, int idx, T value );

#define test_(b,m) \
    if ((b)) { \
        qInfo() << "[OK] " << qPrintable(QString(#b)); \
    } else { \
        errors+=1; qCritical() << "[  ] " << QString(""#b) << " " << m; \
    }

#define test2_(a,b,m) \
    { \
    auto av = a; \
    auto bv = b; \
    if ((av==bv)) { \
        qInfo() << "[OK] " << qPrintable(QString(#a)) << "==" << qPrintable(QString(#b)); \
    } else { \
        errors+=1; \
        qCritical() << "[  ] " << QString(#a) << "==" << QString(#b) << " " << m; \
        qCritical() << "     " << av << " != " << bv << " " << m; \
    } \
    }


#endif // UTIL_H
