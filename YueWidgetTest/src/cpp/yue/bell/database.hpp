

#ifndef YUE_BELL_DATABASE_HPP
#define YUE_BELL_DATABASE_HPP

#include <QObject>
#include <QDebug>
#include <QString>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <exception>

#include "yue/global.h"

namespace yue {
namespace bell {

class DatabaseError : public std::runtime_error
{
public:
    //DatabaseError(std::string msg)
    //    : std::runtime_error(msg) {}
    DatabaseError(QString msg)
        : std::runtime_error(msg.toUtf8().data()) {}
};

class YUECOMMON_EXPORT Database : public QObject
{
    Q_OBJECT
public:
    typedef size_t uid_t;
private:

    static Database* m_instance;

    QSqlDatabase m_db;
public:

    Database(QObject *parent=nullptr);
    ~Database();

    static Database* create( void ) {
        m_instance = new Database();
        return m_instance;
    }
    static QSharedPointer<Database> reconnect( void );
    static Database* instance( void ) {
        return m_instance;
    }

    QSqlDatabase& db() { return m_db; }

    void connect(QString path);

    static bool checked_select(QSqlQuery& query);

private:
    void create_v1_0( void );

};

QVariant toQVariant(size_t value);




} // namespace bell
} // namespace yue


Q_DECLARE_METATYPE(yue::bell::Database::uid_t);
#endif
