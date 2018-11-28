#ifndef YUE_CORE_LIBRARYSEARCHGRAMMAR_HPP
#define YUE_CORE_LIBRARYSEARCHGRAMMAR_HPP


#include "yue/global.h"
#include "yue/core/song.hpp"
#include "yue/core/search/grammar.hpp"
#include "yue/core/search/rules.hpp"
#include "yue/bell/database.hpp"

namespace yue {
namespace bell {

class YUECOMMON_EXPORT LibrarySearchGrammar : public yue::core::SearchGrammar
{
public:

    LibrarySearchGrammar();
    virtual ~LibrarySearchGrammar() = default;

    SqlQuery buildQuery(QStringList select, QString query, QString orderby = "", QSqlDatabase db = QSqlDatabase());
    SqlQuery buildQuery(QStringList select, std::unique_ptr<yue::core::SearchRule> rule, QString orderby = "", QSqlDatabase db = QSqlDatabase());

    SqlQuery buildRemoteQuery(QString query, bool local, bool remote, QSqlDatabase db = QSqlDatabase());
    SqlQuery buildRemoteQuery(std::unique_ptr<yue::core::SearchRule> rule, QSqlDatabase db = QSqlDatabase());

    SqlQuery buildDelete(std::unique_ptr<yue::core::SearchRule> rule, QSqlDatabase db = QSqlDatabase());

private:
    void bindValues(SqlQuery& query, const yue::core::Rule::QueryValues& values);
};

} // bell
} // yue

#endif // YUE_CORE_LIBRARYSEARCHGRAMMAR_HPP
