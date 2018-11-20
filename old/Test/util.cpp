


#include "util.h"

template<>
bool check_value<int>(QSqlQuery& query, int idx, int value )
{
    int tmp = query.value(idx).toInt();
    if (tmp != value) {
        qCritical() << "     Found: " << tmp  << " Expected:" << value;
        return false;
    }
    return true;
}

template<>
bool check_value<QString>(QSqlQuery& query, int idx, QString value )
{
    QString tmp = query.value(idx).toString();
    if (tmp != value) {
        qCritical() << "     Found: " << tmp  << " Expected:" << value;
        return false;
    }
    return true;
}
