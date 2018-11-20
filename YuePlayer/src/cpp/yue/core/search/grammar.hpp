
#ifndef YUE_CORE_SEARCH_PARSER_HPP
#define YUE_CORE_SEARCH_PARSER_HPP

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <iterator>
#include "yue/core/util/dateutil.h"
#include "yue/core/search/rules.hpp"

namespace yue {
namespace core {

class ParseError : public std::runtime_error {
  public:
    ParseError(const std::string &msg = "") : std::runtime_error(msg) {}
    virtual ~ParseError() {}
};

class TokenizeError : public ParseError {
  public:
    TokenizeError(const std::string &msg = "") : ParseError(msg){}
};

// todo: I may want to merge this class into SyntaxNode
// and rename syntax node to SyntaxToken
class StringValue {
  public:
    enum class Mode {
        Unknown,
        Text,
        TextGroup,       /*a list of Text values*/
        BinaryOperator,  /* = == < etc*/
        FlowNot,         /* ! */
        FlowAnd,         /* && */
        FlowParens,      /* special case of && */
        FlowOr,          /* || */
        UnknownOperator, /* tokenized BinaryOperator, but not yet classified */
    };
    static std::string mode(Mode value);

  public:
    StringValue()
        : m_sValue(""), m_iBegin(-1), m_iEnd(-1), m_mode(Mode::Unknown){}
    StringValue(const std::string &str,
                int begin,
                int end,
                Mode mode = Mode::Unknown)
        : m_sValue(str), m_iBegin(begin), m_iEnd(end), m_mode(mode){}
    virtual ~StringValue(){}

    std::string m_sValue;
    int m_iBegin;
    int m_iEnd;
    Mode m_mode;
};

class SyntaxNode {
  public:
    StringValue m_value;
    std::vector<SyntaxNode *> m_children;

  public:
    SyntaxNode() : m_value(), m_children(){};
    SyntaxNode(const StringValue &value) : m_value(value), m_children(){}
    SyntaxNode(std::string str, StringValue::Mode mode)
        : m_value(str, 0, 0, mode), m_children(){}
    ~SyntaxNode(){};

    void pp(int depth = 0) const;

    // add node as a child
    void append(SyntaxNode *node);

    const std::string &text(void) const {
        return m_value.m_sValue;
    }

    const StringValue::Mode &mode(void) const {
        return m_value.m_mode;
    }

    bool operator==(const SyntaxNode &other) {
        if (text() != other.text())
            return false;
        if (other.m_children.size() != m_children.size())
            return false;
        for (size_t i = 0; i < m_children.size(); i++)
            if (*m_children[i] != *other.m_children[i])
                return false;
        return true;
    }
    bool operator!=(const SyntaxNode &other) {
        return !(*this == other);
    }
};

class GrammarState {

  public:
    GrammarState();
    virtual ~GrammarState();

    void append(int index, int start);
    void push();
    void pop();
    void check();
    SyntaxNode *newNode();

    std::string m_token;
    std::back_insert_iterator<std::string> m_inserter;

    bool m_quoted = false;
    bool m_join_special = false;

    SyntaxNode m_root;
    std::vector<SyntaxNode *> m_vNodePool;

    bool stackValid() {
        return m_stack.size() > 0;
    }

  private:
    int m_start = 0;
    std::vector<SyntaxNode *> m_stack;
};

class SyntaxError : public ParseError {
  public:
    SyntaxError(SyntaxNode *node, const std::string &msg = "");
};

class RHSError : public SyntaxError {
  public:
    RHSError(SyntaxNode *node, const std::string &msg = "");
    RHSError(SyntaxNode *node, SyntaxNode *lhs, const std::string &msg = "");
};

class LHSError : public SyntaxError {
  public:
    LHSError(SyntaxNode *node, const std::string &msg = "");
    LHSError(SyntaxNode *node, SyntaxNode *rhs, const std::string &msg = "");
};

class Grammar {
  public:
    enum class CharacterClass {
        whitespace,
        base,
        // control_flow,
        special,
        // negate,
        nest_begin,
        nest_end,
        quote,
        escape,
    };

  public:
    Grammar();
    ~Grammar();

    Grammar(const Grammar &) = delete;             // copy constructor
    Grammar(const Grammar &&) = delete;            // move constructor
    Grammar &operator=(const Grammar &) = delete;  // copy assignment operator
    Grammar &operator=(const Grammar &&) = delete; // move assignment operator

    virtual std::unique_ptr<SearchRule> ruleFromString(const std::string &text);

    SyntaxNode *tokens(void) {
        return &m_state->m_root;
    };

protected:

    virtual void tokenize(std::string text);
    // these "public" private mehtods ...
    // create a friend test class....
    virtual void postProcess(SyntaxNode *root) {
        _postProcess(root);
    }
    // TODO: should return type Rule
    virtual std::unique_ptr<SearchRule> buildRule(SyntaxNode *node);

private:
    void _tokenize(std::string::const_iterator it,
                   std::string::const_iterator end);

    void _postProcess(SyntaxNode *node);
    void _postProcessText(SyntaxNode *node);
    void _postProcessBinary(std::set<std::string> &op,
                            SyntaxNode *node,
                            StringValue::Mode mode);
    void _postProcessUnary(std::set<std::string> &op,
                           SyntaxNode *node,
                           StringValue::Mode mode);


    virtual std::unique_ptr<SearchRule> buildColumnRule(
        SyntaxNode *nd_op, SyntaxNode *nd_column, SyntaxNode *nd_text) = 0;

  protected:
    // SQL table columns
    // each column in the view should be assigned to the set for the
    // corresponding type. By default columns are assumed to be text columns
    // if not found in any of the other sets. Columns explicityl listed
    // in m_col_text will be used for all text rules.
    std::set<std::string> m_col_text;    // text columns
    std::set<std::string> m_col_integer; // column represents a number
    std::set<std::string> m_col_float;   // column represents a number
    std::set<std::string> m_col_date;    // column represents date in seconds
    std::set<std::string> m_col_time; // column represents a duration in seconds
    std::set<std::string> m_col_year; // column represents a year as an integer

    std::map<std::string, std::string> m_col_abbreviations;

    // words that start with a sigil are interpreted as a column name
    // and change the default column form all_text for the current parse level
    std::string m_sigil;
    // all text is a unique string, that does not match an existing column name
    // the string is used to represent a search of all text columns.
    std::string m_all_text;
    // a map which is used to assign unicode code points to a character class
    // the grammar is rather simple, and does not allow for overlap.
    std::map<CharacterClass, std::set<uint32_t>> m_char_classes;

    // sets for operator classes, groups of characters in the special
    // Character Class should match one of theses sets. if not found
    // in all_operators, an error will be thrown for unknown operator
    std::set<std::string> m_flow_not;
    std::set<std::string> m_flow_and;
    std::set<std::string> m_flow_or;
    std::set<std::string> m_binary_operators;
    std::set<std::string> m_all_operators;

    GrammarState *m_state;
};

class SearchGrammarTest;

class SearchGrammar : public Grammar {
  public:
    enum class RuleType {
        PartialString,
        InvertedPartialString,
        Exact,
        InvertedExact,
        RegExpSearch, // not implemented yet
        LessThan,
        LessThanEqual,
        GreaterThan,
        GreaterThanEqual,
    };

  public:
    SearchGrammar();
    ~SearchGrammar() = default;

    std::map<std::string, RuleType> m_operator_types;

    virtual std::string getColumnName(SyntaxNode *nd_column);

    virtual std::unique_ptr<SearchRule> buildColumnRule(SyntaxNode *nd_op,
                                                        SyntaxNode *nd_column,
                                                        SyntaxNode *nd_text);

    virtual std::unique_ptr<SearchRule>
    buildStringColumnRule(SearchGrammar::RuleType type,
                          const std::string &column,
                          SyntaxNode *nd_text);
    virtual std::unique_ptr<SearchRule>
    buildIntegerColumnRule(SearchGrammar::RuleType type,
                           const std::string &column,
                           SyntaxNode *nd_text);
    virtual std::unique_ptr<SearchRule>
    buildDateColumnRule(SearchGrammar::RuleType type,
                        const std::string &column,
                        SyntaxNode *nd_text);

    std::unique_ptr<SearchRule> buildAllTextRule(RuleType type,
                                                 const std::string &text);

    friend class SearchGrammarTest;

protected:
    util::time_point_t m_current_time;
    bool m_recalculate_time;
};

} /*namespace core*/
} /*namespace yue*/

#endif /*  YUE_CORE_SEARCH_PARSER_HPP */
