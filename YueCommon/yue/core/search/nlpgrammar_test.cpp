

#include "yue/core/search/nlpgrammar.hpp"
#include "yue/core/util/yuetest.h"

#include <fstream>

using namespace yue;
using namespace yue::core;


YUE_TEST(nlpgrammar)
{
    YUE_TEST_BEGIN();

    std::vector<SyntaxNode*> text;

    text.push_back(new SyntaxNode("test",StringValue::Mode::Text));
    //text.push_back(new SyntaxNode("artist",StringValue::Mode::Text));
    //text.push_back(new SyntaxNode("is",StringValue::Mode::Text));
    //text.push_back(new SyntaxNode("batman",StringValue::Mode::Text));
    //text.push_back(new SyntaxNode("and",StringValue::Mode::Text));
    //text.push_back(new SyntaxNode("album",StringValue::Mode::Text));
    //text.push_back(new SyntaxNode("is",StringValue::Mode::Text));
    //text.push_back(new SyntaxNode("not",StringValue::Mode::Text));
    //text.push_back(new SyntaxNode("marvel",StringValue::Mode::Text));

    //assert_true(true);



    CYKParser parser;

    std::ifstream ifs ("grammar.txt");
    if (ifs.is_open()) {
        parser.load_cfg(ifs);
    }

    //std::vector<std::string> sentence = {"artist","is","OOV","OOV","and", "title", "is", "OOV","."};
    //std::vector<std::string> sentence = {"artist","is","OOV","OOV","and", "title", "is", "not", "OOV","."};
    //std::vector<std::string> sentence = {"artist","is","OOV","OOV","and", "not", "played", "this","month","."};
    //std::vector<std::string> sentence = {"(", "title", "is", "OOV",")","."};
    //parser.parse(sentence);


    NLPSearchGrammar grammar;
    grammar.test("artist is OOV and not played this month");

    YUE_TEST_END();
}
int main()
{
    std::cout << "hello world" << std::endl;

    yuetest_nlpgrammar();

    return 0;

}