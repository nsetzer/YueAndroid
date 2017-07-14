

#include "yue/core/search/nlpgrammar.hpp"
#include "yue/core/util/yuetest.h"

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

    assert_true(true);

    std::vector<std::string> sentence = {"artist","is","audio","."};
    cykparse(sentence);

    YUE_TEST_END();
}
int main()
{
    std::cout << "hello world" << std::endl;

    yuetest_nlpgrammar();

    return 0;

}