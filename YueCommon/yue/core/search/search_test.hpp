
#ifndef YUE_CORE_SEARCH_SEARCH_TEST_HPP
#define YUE_CORE_SEARCH_SEARCH_TEST_HPP

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "yue/core/search/grammar.hpp"
#include "yue/core/search/rules.hpp"

namespace yue {
namespace core {

class SearchGrammarTest {

  public:
    enum class MatchResult {
        FALSE,
        TRUE,
        LHSError,
        RHSError,
        TokenizeError,
        SyntaxError,
        ParseError,
    };

    SearchGrammarTest();
    ~SearchGrammarTest() = default;

    bool run_test();

  private:
    bool test_sql(const std::string &query,
                  const std::string &sql,
                  const std::vector<std::string> &values);

    bool test_match(const std::string &query,
                    const std::map<std::string, std::string> &item,
                    MatchResult chk);

    bool test_tokenizer(const std::string &text, SyntaxNode *expected);

    bool test_token_parser(const std::string &text, SyntaxNode *expected);

    SearchGrammar m_grammar;
};

void test_grammar();

} /*namespace core*/
} /*namespace yue*/

#endif /*  YUE_CORE_SEARCH_SEARCH_TEST_HPP */
