#include "yue/bell/LibrarySearchGrammar.hpp"

namespace yue {
namespace bell {


LibrarySearchGrammar::LibrarySearchGrammar()
    : SearchGrammar()
{
    for (const char* v : yue::core::Song::StringKeys) {
        m_col_text.insert( v );
    }
    for (const char* v : yue::core::Song::IntegerKeys) {
        m_col_integer.insert( v );
    }

    //m_col_float;
    //m_col_date;
    //m_col_time;
    //m_col_year;

    m_all_text = yue::core::Song::all_text;

    for (auto& it : yue::core::Song::abbreviations) {
        m_col_abbreviations[it.first] = it.second;
    }
}

SqlQuery LibrarySearchGrammar::buildQuery(QStringList select, QString query, QString orderby, QSqlDatabase db)
{
    auto rule = ruleFromString(std::string(query.toUtf8()));
    return buildQuery(select, std::move(rule), orderby, db);
}

SqlQuery LibrarySearchGrammar::buildQuery(QStringList select, std::unique_ptr<yue::core::SearchRule> rule, QString orderby, QSqlDatabase db)
{
    typedef yue::core::util::Variant::Type Type;
    yue::core::Rule::QueryValues values;
    SqlQuery sqlquery(db);

    QString sql = "SELECT " + select.join(", ");
    sql += " FROM library ";

    if (rule) {
        std::string s = rule->toSql(values);
        if (s.size() > 0) {
            sql += QString(" WHERE ") + s.c_str();
        }
    }

    if (orderby.size()>0) {
        sql += " ORDER BY " + orderby;
    }
    /*
    QString s = "SELECT artist, album, title, uid, artist_key, album_index "
                "FROM library "
                "ORDER BY artist_key COLLATE NOCASE, "
                "album COLLATE NOCASE, "
                "album_index, "
                "title COLLATE NOCASE ASC";*/

    sqlquery.prepare( sql );
    bindValues(sqlquery, values);

    return sqlquery;
}

SqlQuery LibrarySearchGrammar::buildRemoteQuery(QString query, bool local, bool remote, QSqlDatabase db)
{

    std::unique_ptr<yue::core::SearchRule> rule;
    rule = ruleFromString(std::string(query.toUtf8()));

    if (local && remote) {
        // if both local and remote is true, return all files
        rule = ruleFromString(std::string(query.toUtf8()));
    } else {
        rule = ruleFromString(std::string(query.toUtf8()));
        // if remote is true, return only remote files
        // otherwise return local files
        /*int value = (remote)?1:0;
        rule = std::move(std::unique_ptr<yue::core::SearchRule>(new yue::core::AndSearchRule({
                std::unique_ptr<yue::core::SearchRule>(
                    new yue::core::ExactSearchRule<int>(yue::core::Song::remote, value)),
                ruleFromString(std::string(query.toUtf8()))})));
                */
    }

    return buildRemoteQuery(std::move(rule), db);
}

SqlQuery LibrarySearchGrammar::buildRemoteQuery(std::unique_ptr<yue::core::SearchRule> rule, QSqlDatabase db)
{
    typedef yue::core::Song Song;
    yue::core::Rule::QueryValues values;
    SqlQuery sqlquery(db);

    QStringList lst;
    lst  << Song::artist       // 0
         << Song::album        // 1
         << Song::title        // 2
         << Song::rating       // 3
         << Song::uid          // 4
         << Song::artist_key   // 5
         << Song::album_index; // 6

    QString sql = "SELECT " + lst.join(", ") + " FROM songs ";

    if (rule) {
        std::string s = rule->toSql(values);
        if (s.size() > 0) {
            sql += QString(" WHERE ") + s.c_str();
        }
    }

    sql += " ORDER BY artist_key COLLATE NOCASE, album COLLATE NOCASE, album_index, title COLLATE NOCASE ASC;";

    sqlquery.prepare( sql );

    bindValues(sqlquery, values);

    return sqlquery;
}

SqlQuery LibrarySearchGrammar::buildDelete(std::unique_ptr<yue::core::SearchRule> rule, QSqlDatabase db)
{
    SqlQuery sqlquery(db);
    typedef yue::core::util::Variant::Type Type;
    yue::core::Rule::QueryValues values;
    QString sql = "DELETE FROM songs";

    if (rule) {
        std::string s = rule->toSql(values);
        if (s.size() > 0) {
            sql += QString(" WHERE (") + s.c_str() + QString(")");
        }
    }

    sqlquery.prepare( sql );
    for (auto& v : values) {
        switch (v->type()) {
        case Type::Int:
            sqlquery.addBindValue(v->toInt());
            break;
        case Type::Float:
            sqlquery.addBindValue(v->toFloat());
            break;
        case Type::String:
            sqlquery.addBindValue(v->toString().c_str());
            break;
        default:
            throw std::runtime_error("unknown type in rule");
        }
    }

    return sqlquery;
}

void LibrarySearchGrammar::bindValues(SqlQuery& query, const yue::core::Rule::QueryValues& values)
{
    typedef yue::core::util::Variant::Type Type;

    for (auto& v : values) {
        switch (v->type()) {
        case Type::Int:
            query.addBindValue(v->toInt());
            break;
        case Type::Float:
            query.addBindValue(v->toFloat());
            break;
        case Type::String:
            query.addBindValue(v->toString().c_str());
            break;
        default:
            throw std::runtime_error("unknown type in rule");
        }
    }
}

} // bell
} // yue
