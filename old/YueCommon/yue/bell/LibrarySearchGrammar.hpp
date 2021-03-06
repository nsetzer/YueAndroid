#ifndef YUE_CORE_LIBRARYSEARCHGRAMMAR_HPP
#define YUE_CORE_LIBRARYSEARCHGRAMMAR_HPP

#include <QSqlQuery>
#include "yue/global.h"
#include "yue/core/song.hpp"
#include "yue/core/search/grammar.hpp"
#include "yue/core/search/rules.hpp"

namespace yue {
namespace bell {

class YUECOMMON_EXPORT LibrarySearchGrammar : public yue::core::SearchGrammar
{
public:

    LibrarySearchGrammar();
    ~LibrarySearchGrammar() = default;

    QSqlQuery buildQuery(QStringList select, QString query, QString orderby = "");
};

} // bell
} // yue

#endif // YUE_CORE_LIBRARYSEARCHGRAMMAR_HPP
