
#include <iostream>

#include "yue/core/search/search_test.hpp"
#include "yue/core/song.hpp"

namespace yue {
namespace core {

SearchGrammarTest::SearchGrammarTest() : m_grammar() {
    // construct a grammar with a couple sample columns
    m_grammar.m_recalculate_time = false;
    m_grammar.m_current_time = util::currentTimePoint();

    m_grammar.m_col_text.insert("artist");
    m_grammar.m_col_text.insert("album");
    m_grammar.m_col_text.insert("title");

    m_grammar.m_col_date.insert("date");

    m_grammar.m_col_integer.insert("play_count");

    m_grammar.m_col_abbreviations["art"] = "artist";
    m_grammar.m_col_abbreviations["artist"] = "artist";
    m_grammar.m_col_abbreviations["alb"] = "album";
    m_grammar.m_col_abbreviations["album"] = "album";
    m_grammar.m_col_abbreviations["ttl"] = "title";
    m_grammar.m_col_abbreviations["title"] = "title";
    m_grammar.m_col_abbreviations["pcnt"] = "play_count";
    m_grammar.m_col_abbreviations["play_count"] = "play_count";
    m_grammar.m_col_abbreviations["date"] = "date";
    m_grammar.m_col_abbreviations["text"] = m_grammar.m_all_text;
    m_grammar.m_col_abbreviations[m_grammar.m_all_text] = m_grammar.m_all_text;
}

bool
SearchGrammarTest::run_test() {

    int i_failed = 0;
    int i_success = 0;
#define run(x)       \
    if ((x)) {       \
        ++i_success; \
    } else {         \
        ++i_failed;  \
    }
    {
        SyntaxNode root("()", StringValue::Mode::FlowAnd);
        SyntaxNode nd1("abc", StringValue::Mode::Text);
        root.append(&nd1);
        run(test_tokenizer("abc", &root));
    }

    {
        SyntaxNode root("()", StringValue::Mode::FlowAnd);
        SyntaxNode nd1("abcdef", StringValue::Mode::Text);
        root.append(&nd1);
        run(test_tokenizer("abc\"def\"", &root));
    }

    {
        SyntaxNode root("()", StringValue::Mode::FlowAnd);
        SyntaxNode nd1("abcdef", StringValue::Mode::Text);
        SyntaxNode nd2("||", StringValue::Mode::FlowOr);
        SyntaxNode nd3("x", StringValue::Mode::Text);
        root.append(&nd1);
        root.append(&nd2);
        root.append(&nd3);
        run(test_tokenizer("abc\"def\"||x", &root));
    }

    {
        SyntaxNode root("()", StringValue::Mode::FlowAnd);
        SyntaxNode nd1("x", StringValue::Mode::Text);
        SyntaxNode nd2("||", StringValue::Mode::FlowOr);
        SyntaxNode nd3("abc", StringValue::Mode::Text);
        root.append(&nd1);
        root.append(&nd2);
        root.append(&nd3);
        run(test_tokenizer("x||\"abc\"", &root));
    }

    {
        SyntaxNode root("()", StringValue::Mode::FlowAnd);
        SyntaxNode nd11("||", StringValue::Mode::FlowOr);
        SyntaxNode nd21("=", StringValue::Mode::BinaryOperator);
        SyntaxNode nd22("=", StringValue::Mode::BinaryOperator);
        SyntaxNode nd31("all_text", StringValue::Mode::Text);
        SyntaxNode nd32("x", StringValue::Mode::Text);
        SyntaxNode nd33("all_text", StringValue::Mode::Text);
        SyntaxNode nd34("y", StringValue::Mode::Text);
        root.append(&nd11);
        nd11.append(&nd21);
        nd11.append(&nd22);
        nd21.append(&nd31);
        nd21.append(&nd32);
        nd22.append(&nd33);
        nd22.append(&nd34);
        run(test_token_parser("x||y", &root));
        run(test_token_parser("x || y", &root));
    }

    {
        SyntaxNode root("()", StringValue::Mode::FlowAnd);
        SyntaxNode nd11("||", StringValue::Mode::FlowOr);
        SyntaxNode nd21(">", StringValue::Mode::BinaryOperator);
        SyntaxNode nd22("<", StringValue::Mode::BinaryOperator);
        SyntaxNode nd31("a", StringValue::Mode::Text);
        SyntaxNode nd32("b", StringValue::Mode::Text);
        SyntaxNode nd33("c", StringValue::Mode::Text);
        SyntaxNode nd34("d", StringValue::Mode::Text);
        root.append(&nd11);
        nd11.append(&nd21);
        nd11.append(&nd22);
        nd21.append(&nd31);
        nd21.append(&nd32);
        nd22.append(&nd33);
        nd22.append(&nd34);
        run(test_token_parser("a>b||c<d", &root));
        run(test_token_parser(" a > b || c < d ", &root));
    }

    {
        // check hoisting of values
        SyntaxNode root("()", StringValue::Mode::FlowAnd);
        SyntaxNode nd01("()", StringValue::Mode::FlowAnd);
        SyntaxNode nd11("||", StringValue::Mode::FlowOr);
        SyntaxNode nd21(">", StringValue::Mode::BinaryOperator);
        SyntaxNode nd22("<", StringValue::Mode::BinaryOperator);
        SyntaxNode nd31("a", StringValue::Mode::Text);
        SyntaxNode nd32("b", StringValue::Mode::Text);
        SyntaxNode nd33("c", StringValue::Mode::Text);
        SyntaxNode nd34("d", StringValue::Mode::Text);
        root.append(&nd01);
        nd01.append(&nd11);
        nd11.append(&nd21);
        nd11.append(&nd22);
        nd21.append(&nd31);
        nd21.append(&nd32);
        nd22.append(&nd33);
        nd22.append(&nd34);
        run(test_token_parser(" (a>b||c<d) ", &root));
    }

    {
        std::map<std::string, std::string> item;
        item["artist"] = "test";
        item["title"] = "test";
        item["album"] = "test";
        item["play_count"] = "5";
        run(test_match("", item, MatchResult::TRUE));
        run(test_match(" ", item, MatchResult::TRUE));
        run(test_match("artist=test", item, MatchResult::TRUE));
        run(test_match("artist = test", item, MatchResult::TRUE));
        run(test_match(" \"test\" ", item, MatchResult::TRUE));
        run(test_match(" te\"st\" ", item, MatchResult::TRUE));
        run(test_match(" \"te\"st ", item, MatchResult::TRUE));
        run(test_match("artist=", item, MatchResult::RHSError));
        run(test_match("=test", item, MatchResult::LHSError));
        run(test_match(" ! test ", item, MatchResult::FALSE));
        run(test_match(" ! ", item, MatchResult::RHSError));
        run(test_match(" x && ", item, MatchResult::RHSError));
        run(test_match(" x || ", item, MatchResult::RHSError));
        run(test_match(" && y ", item, MatchResult::LHSError));
        run(test_match(" || y ", item, MatchResult::LHSError));
        run(test_match(" artist = test && title = test ", item,
                       MatchResult::TRUE));
        run(test_match(" (abc ", item, MatchResult::TokenizeError));
        run(test_match(" abc) ", item, MatchResult::TokenizeError));
        run(test_match(" abc\" ", item, MatchResult::TokenizeError));
        run(test_match(" play_count = 5  ", item, MatchResult::TRUE));
        run(test_match(" play_count = 0  ", item, MatchResult::FALSE));
        run(test_match(" play_count != 5 ", item, MatchResult::FALSE));
        run(test_match(" play_count != 0 ", item, MatchResult::TRUE));
        run(test_match(" play_count > 4  ", item, MatchResult::TRUE));
        run(test_match(" play_count >= 4 ", item, MatchResult::TRUE));
        run(test_match(" play_count < 6  ", item, MatchResult::TRUE));
        run(test_match(" play_count <= 6 ", item, MatchResult::TRUE));
        run(test_match(" play_count < 5  ", item, MatchResult::FALSE));
        run(test_match(" play_count > 5  ", item, MatchResult::FALSE));
        run(test_match(" play_count =~ 5 ", item, MatchResult::SyntaxError));
        run(test_match(" artist != \"<!>\" ", item, MatchResult::TRUE));
        run(test_match(" what != what ", item, MatchResult::SyntaxError));

        // things that probably should fail, but not for any specific reason
        // illegal construct -- just so happens to return RHSError today...
        run(test_match(" artist != <\"!\"> ", item, MatchResult::RHSError));
        // construct is not yet supported
        run(test_match(" artist=~foo", item, MatchResult::SyntaxError));
        run(test_match(" (artist = test || album = test) ", item,
                       MatchResult::TRUE));
        run(test_match(" artist = test && album = abc    ", item,
                       MatchResult::FALSE));
        run(test_match(" artist = test (album = test)    ", item,
                       MatchResult::TRUE));
        run(test_match(" artist = test && (album = abc)  ", item,
                       MatchResult::FALSE));
        run(test_match(" (artist = test) && album = abc  ", item,
                       MatchResult::FALSE));
        run(test_match(" (artist = test) && (album = abc)", item,
                       MatchResult::FALSE));
        run(test_match(" artist = test || (album = abc)  ", item,
                       MatchResult::TRUE));
    }

    {
        // string types
        run(test_sql("artist=test", "artist LIKE ?",
                     {
                         "%test%",
                     }));

        run(test_sql("artist=test", "artist LIKE ?",
                     {
                         "%test%",
                     }));
        run(test_sql("artist!=test", "artist NOT LIKE ?",
                     {
                         "%test%",
                     }));
        run(test_sql("artist<test", "artist < ?",
                     {
                         "test",
                     }));
        run(test_sql("artist<=test", "artist <= ?",
                     {
                         "test",
                     }));
        run(test_sql("artist>test", "artist > ?",
                     {
                         "test",
                     }));
        run(test_sql("artist>=test", "artist >= ?",
                     {
                         "test",
                     }));

        // integer types
        run(test_sql("play_count=123", "play_count == ?",
                     {
                         "123",
                     }));
        run(test_sql("play_count=123", "play_count == ?",
                     {
                         "123",
                     }));
        run(test_sql("play_count!=123", "play_count != ?",
                     {
                         "123",
                     }));
        run(test_sql("play_count<123", "play_count < ?",
                     {
                         "123",
                     }));
        run(test_sql("play_count<=123", "play_count <= ?",
                     {
                         "123",
                     }));
        run(test_sql("play_count>123", "play_count > ?",
                     {
                         "123",
                     }));
        run(test_sql("play_count>=123", "play_count >= ?",
                     {
                         "123",
                     }));

        // check abbreviations
        run(test_sql("art=test", "artist LIKE ?",
                     {
                         "%test%",
                     }));
        run(test_sql("pcnt=5", "play_count == ?",
                     {
                         "5",
                     }));

        run(test_sql("play_count < 4 || play_count > 6",
                     "(play_count < ? OR play_count > ?)", {"4", "6"}));

        run(test_sql("play_count > 4 && play_count < 6",
                     "(play_count > ? AND play_count < ?)", {"4", "6"}));

        // this depends on order of sets ...
        run(test_sql("test", "(album LIKE ? OR artist LIKE ? OR title LIKE ?)",
                     {"%test%", "%test%", "%test%"}));

        //
        util::ydate_t date = util::extractDate(m_grammar.m_current_time);
        int value_lo = util::dateToEpochTime(date);
        date = dateDelta(date,0,0,1);
        int value_hi = util::dateToEpochTime(date);
        run(test_sql("date = 0d", "date BETWEEN ? AND ?",
                     {util::toString<int>(value_lo),
                      util::toString<int>(value_hi)}));

    }

    int i_total = i_success + i_failed;
    if (i_failed > 0) {
        std::cout << i_failed << "/" << i_total << " tests failed!"
                  << std::endl;

    } else {
        std::cout << i_total << " tests passed!" << std::endl;
    }
    return true;
}


bool
SearchGrammarTest::test_sql(const std::string &query,
                            const std::string &sql,
                            const std::vector<std::string> &values) {

    try {
        auto rule = m_grammar.ruleFromString(query);

        Rule::QueryValues result_val;
        std::string result_str = rule->toSql(result_val);

        // check that the query values are in the correct order.
        bool bValsMatch = result_val.size() == values.size();
        for (size_t i = 0; bValsMatch && i < result_val.size(); i++) {
            if (result_val[i]->toString() != values[i]) {
                bValsMatch = false;
            }
        }

        if (!bValsMatch || result_str != sql) {
            std::cout << "[  ] \"" << sql << "\"" << std::endl;
            std::cout << "     \"" << result_str << "\"" << std::endl;
            // print out query values allowing for missing items.
            size_t n = result_val.size() > values.size() ? result_val.size()
                                                         : values.size();

            std::cout << "arguments -- expected | actual" << std::endl;
            for (size_t i = 0; i < n; i++) {
                std::cout << i << ": "
                          << ((i < values.size()) ? values[i] : "none")
                          << " != "
                          << ((i < result_val.size()) ? result_val[i]->toString() : "none")
                          << std::endl;
            }
            return false;
        }
        std::cout << "[ok] " << result_str << std::endl;

        return true;
    } catch (ParseError &e) {
        std::cout << "[ x] " << sql << std::endl;
        std::cout << "Exception:" << std::endl << e.what() << std::endl;
    }

    return false;
}


bool
SearchGrammarTest::test_tokenizer(const std::string &text,
                                  SyntaxNode *expected) {
    try {
        m_grammar.tokenize(text);
        // m_grammar.postProcess();
        SyntaxNode *result = m_grammar.tokens();

        if (*result != *expected) {
            std::cout << "[  ] " << text << " Incorrect Parse Tree"
                      << std::endl;

            std::cout << "Found:" << std::endl;
            result->pp();
            std::cout << "Expected:" << std::endl;
            expected->pp();
        } else {
            std::cout << "[ok] " << text << std::endl;
            return true;
        }

    } catch (ParseError &e) {
        std::cout << "[ x] " << text << std::endl;
        std::cout << "Exception:" << std::endl << e.what() << std::endl;
        std::cout << "---" << std::endl;
        m_grammar.tokens()->pp();
        std::cout << "---" << std::endl;
        expected->pp();
        std::cout << "---" << std::endl;
    }

    return false;
}

bool
SearchGrammarTest::test_token_parser(const std::string &text,
                                     SyntaxNode *expected) {
    try {
        m_grammar.tokenize(text);
        m_grammar.postProcess();
        SyntaxNode *result = m_grammar.tokens();

        if (*result != *expected) {
            std::cout << "[  ] " << text << " Incorrect Parse Tree"
                      << std::endl;

            std::cout << "Found:" << std::endl;
            result->pp();
            std::cout << "Expected:" << std::endl;
            expected->pp();
        } else {
            std::cout << "[ok] " << text << std::endl;
            return true;
        }
    } catch (ParseError &e) {
        std::cout << "[ x] " << text << std::endl;
        std::cout << "Exception:" << std::endl << e.what() << std::endl;
        std::cout << "---" << std::endl;
        m_grammar.tokens()->pp();
        std::cout << "---" << std::endl;
        expected->pp();
        std::cout << "---" << std::endl;
    }

    return false;
}

bool
SearchGrammarTest::SearchGrammarTest::test_match(
    const std::string &query,
    const std::map<std::string, std::string> &item,
    MatchResult chk) {
#define TEST_CATCH_EXCEPTION(X)                                              \
    catch (X & e) {                                                          \
        if (chk == MatchResult::X) {                                         \
            std::cout << "[ok] " << query << " - " << e.what() << std::endl; \
            return true;                                                     \
        }                                                                    \
        std::cout << "[ x] " << query << std::endl;                          \
        std::cout << "Exception:" << std::endl << e.what() << std::endl;     \
        m_grammar.tokens()->pp();                                            \
    }
    try {

        std::unique_ptr<SearchRule> rule = m_grammar.ruleFromString(query);

        bool bMatch = rule->check(item, true);
        if (bMatch && chk == MatchResult::TRUE) {
            std::cout << "[ok] " << query << " - returned: TRUE" << std::endl;
            return true;
        }
        if (!bMatch && chk == MatchResult::FALSE) {
            std::cout << "[ok] " << query << " - returned: FALSE" << std::endl;
            return true;
        }

        std::cout << "[  ] " << query << std::endl;

        SyntaxNode *result = m_grammar.tokens();
        std::cout << "---" << std::endl;
        std::cout << "query:" << query << std::endl;
        std::cout << "input:" << std::endl;
        for (auto &it : item) {
            std::cout << "  " << it.first << "=" << it.second << std::endl;
        }
        std::cout << "---" << std::endl;
        result->pp();
        std::cout << "---" << std::endl;
        std::cout << "sql: " << rule->toSqlString() << std::endl;
        std::cout << "---" << std::endl;
    }
    TEST_CATCH_EXCEPTION(LHSError)
    TEST_CATCH_EXCEPTION(RHSError)
    TEST_CATCH_EXCEPTION(TokenizeError)
    TEST_CATCH_EXCEPTION(SyntaxError)
    TEST_CATCH_EXCEPTION(ParseError)
    return false;
}

void
test_grammar() {

    SearchGrammar grammar;
    SearchGrammarTest test;
    test.run_test();

    try {

        // std::string text = "artist=stone  !(pcnt > 7)";
        std::string text = "";
        std::unique_ptr<SearchRule> rule = grammar.ruleFromString(text);
        if (rule) {
            std::cout << "input:  " << text << std::endl;
            std::cout << "output: " << rule->toSqlString() << std::endl;
        }

        Song s;
        s.set<Song::str_t>("artist", "stoxne");

        std::map<std::string, std::string> mData;
        s.toMap(mData);
        std::cout << rule->check(mData, true) << std::endl;

    } catch (ParseError &e) {
        std::cout << "Error:" << std::endl << e.what() << std::endl;
    }
}

} /*namespace core*/
} /*namespace yue*/