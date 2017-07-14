
#ifndef YUE_CORE_SEARCH_NLPGRAMMAR_HPP
#define YUE_CORE_SEARCH_NLPGRAMMAR_HPP

#include "yue/core/util/dateutil.h"
#include "yue/core/search/rules.hpp"
#include "yue/core/search/grammar.hpp"

namespace yue {
namespace core {


class NLPContext
{
public:
    NLPContext();
    ~NLPContext() = default;
};


void cykparse(std::vector<std::string> sentence);

} // namespace core
} // namespace yue

#endif // YUE_CORE_SEARCH_NLPGRAMMAR_HPP