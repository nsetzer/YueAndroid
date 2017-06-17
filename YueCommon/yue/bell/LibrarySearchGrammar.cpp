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


} // bell
} // yue
