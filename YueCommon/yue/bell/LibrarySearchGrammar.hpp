#ifndef YUE_CORE_LIBRARYSEARCHGRAMMAR_HPP
#define YUE_CORE_LIBRARYSEARCHGRAMMAR_HPP


#include "yue/global.h"
#include "yue/core/song.hpp"
#include "yue/core/search/grammar.hpp"

namespace yue {
namespace bell {

class YUECOMMON_EXPORT LibrarySearchGrammar : public yue::core::SearchGrammar
{
public:

    LibrarySearchGrammar();
    ~LibrarySearchGrammar() = default;
};

} // bell
} // yue

#endif // YUE_CORE_LIBRARYSEARCHGRAMMAR_HPP
