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

QSqlQuery LibrarySearchGrammar::buildQuery(QStringList select, QString query, QString orderby, QSqlDatabase db)
{
    auto rule = ruleFromString(std::string(query.toUtf8()));
    return buildQuery(select, std::move(rule), orderby, db);
}

QSqlQuery LibrarySearchGrammar::buildQuery(QStringList select, std::unique_ptr<yue::core::SearchRule> rule, QString orderby, QSqlDatabase db)
{
    typedef yue::core::util::Variant::Type Type;
    yue::core::Rule::QueryValues values;
    QSqlQuery sqlquery(db);

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

QSqlQuery LibrarySearchGrammar::buildDelete(std::unique_ptr<yue::core::SearchRule> rule, QSqlDatabase db)
{
    QSqlQuery sqlquery(db);
    typedef yue::core::util::Variant::Type Type;
    yue::core::Rule::QueryValues values;
    QString sql = "DELETE FROM library";

    if (rule) {
        std::string s = rule->toSql(values);
        if (s.size() > 0) {
            sql += QString(" WHERE ") + s.c_str();
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


} // bell
} // yue
