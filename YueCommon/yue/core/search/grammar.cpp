

#include <iostream>
#include "yue/core/search/grammar.hpp"
#include "yue/alien/utf8/utf8.h"
#include "yue/core/util/dateutil.h"

namespace yue {
namespace core {

std::string
StringValue::mode(StringValue::Mode value) {
    switch (value) {
    case StringValue::Mode::Text:
        return "Text";
    case StringValue::Mode::TextGroup:
        return "Group";
    case StringValue::Mode::BinaryOperator:
        return "Binary";
    case StringValue::Mode::FlowNot:
        return "Not";
    case StringValue::Mode::FlowAnd:
        return "And";
    case StringValue::Mode::FlowParens:
        return "Parenthesis";
    case StringValue::Mode::FlowOr:
        return "Or";
    case StringValue::Mode::UnknownOperator:
        return "UnknownOperator";
    case StringValue::Mode::Unknown:
    default:
        return "Unknown";
    }
}

SyntaxError::SyntaxError(SyntaxNode *node, const std::string &msg)
    : ParseError(msg + " at position " +
                 util::toString<int>(node->m_value.m_iBegin)) {}

RHSError::RHSError(SyntaxNode *node, const std::string &msg)
    : SyntaxError(node,
                  msg + " on right hand side of " + node->m_value.m_sValue) {}

RHSError::RHSError(SyntaxNode *node, SyntaxNode *lhs, const std::string &msg)
    : SyntaxError(node,
                  msg + " on right hand side of " + lhs->text() + " " +
                      node->text()) {}

LHSError::LHSError(SyntaxNode *node, const std::string &msg)
    : SyntaxError(node,
                  msg + " on left hand side of " + node->m_value.m_sValue) {}

LHSError::LHSError(SyntaxNode *node, SyntaxNode *rhs, const std::string &msg)
    : SyntaxError(node,
                  msg + " on left hand side of " + node->text() + " " +
                      rhs->text()) {}

void
SyntaxNode::append(SyntaxNode *node) {
    m_children.push_back(node);
}

void
SyntaxNode::pp(int depth /*=0*/) const {
    for (int i = 0; i < depth; i++) {
        std::cout << "  ";
    }
    std::cout << m_value.m_sValue << " (" << m_value.m_iBegin << "-"
              << m_value.m_iEnd << ")\t:: " << StringValue::mode(m_value.m_mode)
              << " " << static_cast<int>(m_children.size()) << std::endl;

    for (SyntaxNode *child : m_children) {
        child->pp(depth + 1);
    }
}

GrammarState::GrammarState()
    : m_token(""), m_inserter(std::back_inserter(m_token)) {
    m_root.m_value.m_sValue = "()";
    m_root.m_value.m_mode = StringValue::Mode::FlowParens;
    m_stack.push_back(&m_root);
}

GrammarState::~GrammarState() {
    for (SyntaxNode *nd : m_vNodePool) {
        delete nd;
    }
}

void
GrammarState::append(int index, int start) {
    if (!m_token.empty()) {
        StringValue::Mode mode = (m_join_special)
                                     ? StringValue::Mode::UnknownOperator
                                     : StringValue::Mode::Text;
        SyntaxNode *temp = newNode();
        StringValue value(m_token, m_start, index, mode);
        temp->m_value = value;
        m_stack.back()->append(temp);
    }

    m_join_special = false;
    m_quoted = false;

    m_token = "";
    m_inserter = std::back_inserter(m_token);
    m_start = start;
}

void
GrammarState::push() {
    SyntaxNode *temp = newNode();
    // we always push an && node by default
    temp->m_value.m_sValue = "()";
    temp->m_value.m_mode = StringValue::Mode::FlowParens;
    m_stack.back()->append(temp);
    m_stack.push_back(temp);
}

void
GrammarState::pop() {
    SyntaxNode *nd = m_stack.back();
    // when stack nodes are pushed they are not given the
    // range for the tokens the cover. update that information
    // here, to improve debugging.
    if (!nd->m_children.empty()) {
        nd->m_value.m_iBegin = nd->m_children.front()->m_value.m_iBegin;
        nd->m_value.m_iEnd = nd->m_children.back()->m_value.m_iEnd;
    }
    m_stack.pop_back();
}

void
GrammarState::check() {
    if (m_stack.empty())
        throw TokenizeError("Empty stack (check parenthesis)");
    if (m_stack.size() > 1)
        throw TokenizeError("Unterminated Parenthesis");
    if (m_quoted)
        throw TokenizeError("Unterminated Double Quote");
}

SyntaxNode *
GrammarState::newNode() {
    m_vNodePool.push_back(new SyntaxNode());
    return m_vNodePool.back();
}

Grammar::Grammar() {
    m_state = nullptr;
}

Grammar::~Grammar() {
    if (m_state != nullptr)
        delete m_state;
}

std::unique_ptr<SearchRule>
Grammar::ruleFromString(const std::string &text) {
    /*
    first, split the input string into tokens.
    this is context free, using simple rules to split the string
    and combine characters together
    */
    tokenize(text);
    /*
    post process the tokens, assign meaning to the indivudal tokens
    and create new nodes in the parse tree as needed to fill in gaps.
    check for errors when a token does not make sense in the given context
    */
    postProcess(&m_state->m_root);
    /*
    walk the parse tree and generate rules
    */
    std::unique_ptr<SearchRule> rule = buildRule(&m_state->m_root);
    // if null, no error, return BlankSearch
    return rule;
}

void
Grammar::tokenize(std::string text) {
    // create a new grammar state for
    if (m_state != nullptr)
        delete m_state;

    m_state = new GrammarState();

    try {
        _tokenize(text.begin(), text.end());
    } catch (utf8::not_enough_room &) {

        // the last codepoint is malformed?
        throw ParseError("error parsing input string");
    }
}

void
Grammar::_tokenize(std::string::const_iterator it,
                   std::string::const_iterator end) {
    // index of the current character being processed
    int idx = -1;

// check if a utf-8 character (uint32_t) exists in the set of operators
// for a given operator class
#define exists_in(c, op) \
    m_char_classes[Grammar::CharacterClass::op].count(c) != 0
// increment the character index and advance the iterator
// return the next character in the sequence.
#define next() (idx++, utf8::next(it, end))

    while (it != end && m_state->stackValid()) {
        uint32_t c = next();

        if (exists_in(c, escape)) {
            // if an escape character is found append the next character
            // to the token without processing it.
            c = next();
            utf8::append(c, m_state->m_inserter);
        } else if (!m_state->m_quoted) {
            if (exists_in(c, quote)) {
                // if not quoted and a quote is found
                // push the existing token and continue in a quoted mode.
                if (m_state->m_join_special)
                    m_state->append(idx, idx + 1);
                m_state->m_quoted = true;
            } else if (exists_in(c, nest_begin)) {
                // nest begin tokens push the token stack
                m_state->append(idx, idx + 1);
                m_state->push();
            } else if (exists_in(c, nest_end)) {
                // nest end characters pop from the stack
                m_state->append(idx, idx + 1);
                m_state->pop();
            } else if (exists_in(c, whitespace)) {
                // white space is used to separate tokens, when not quoted
                m_state->append(idx, idx + 1);
            } else {
                // when not quoted, append all characters together
                // to form a token. distinguish between characters that
                // are used for operators, and join those separatley
                bool s = exists_in(c, special);
                if (s != m_state->m_join_special)
                    m_state->append(idx, idx);
                m_state->m_join_special = s;
                utf8::append(c, m_state->m_inserter);
            }
        } else { // is quoted
            if (exists_in(c, quote)) {
                // disable the quoted state
                m_state->m_quoted = false;
            } else {
                // the character has no special meaning,
                // append to the current token
                utf8::append(c, m_state->m_inserter);
            }
        }
    }

    m_state->check();
    m_state->append(idx, idx);

#undef exists_in
#undef next
}

void
Grammar::_postProcess(SyntaxNode *node) {
    if (node->m_children.empty())
        return;
    // todo: handle empty ndoes, such as && or || with no children
    // should they be removed?

    for (SyntaxNode *child : node->m_children) {
        // check for syntax errors
        // look for operators we don't know how to handle
        if (child->mode() == StringValue::Mode::UnknownOperator &&
            m_all_operators.count(child->text()) == 0) {
            throw ParseError("Unknown Operator...");
        }

        // descend into child nodes
        _postProcess(child);
    }

    // collect binary operators
    _postProcessBinary(m_binary_operators, node,
                       StringValue::Mode::BinaryOperator);

    // collect the remaining text, which implies all_text mode.
    _postProcessText(node);

    // process unary operators in right-to-left order
    _postProcessUnary(m_flow_not, node, StringValue::Mode::FlowNot);

    // process binary operators which effect flow
    _postProcessBinary(m_flow_and, node, StringValue::Mode::FlowAnd);
    _postProcessBinary(m_flow_or, node, StringValue::Mode::FlowOr);
}

void
Grammar::_postProcessText(SyntaxNode *node) {
    std::string column = m_all_text;

    for (size_t i = 0; i < node->m_children.size();) {
        SyntaxNode *child = node->m_children[i];

        if (child->m_value.m_mode == StringValue::Mode::Text) {
            std::string &text = child->m_value.m_sValue;
            if (text == m_sigil) {
                column =
                    text.substr(m_sigil.size(), text.size() - m_sigil.size());
                node->m_children.erase(node->m_children.begin() + i);
            } else {
                // replace the existing node with a. binary operator
                // one that matches all_text for the given text
                SyntaxNode *nd_op = m_state->newNode();
                SyntaxNode *nd_field = m_state->newNode();
                int b = child->m_value.m_iBegin;
                int e = child->m_value.m_iEnd;
                StringValue op("=", b, e, StringValue::Mode::BinaryOperator);
                nd_op->m_value = op;
                StringValue field(column, b, e, StringValue::Mode::Text);
                nd_field->m_value = field;
                nd_op->append(nd_field);
                nd_op->append(child);
                node->m_children[i] = nd_op;
                i++;
            }
        } else {
            i++;
        }
    }
}

void
Grammar::_postProcessUnary(std::set<std::string> &op,
                           SyntaxNode *node,
                           StringValue::Mode mode) {
    for (size_t j = node->m_children.size(); j > 0; --j) {
        size_t i = j - 1;
        SyntaxNode *child = node->m_children[i];

        if (op.count(child->m_value.m_sValue) > 0) {
            child->m_value.m_mode = mode;

            if (i + 1 >= node->m_children.size()) {
                throw RHSError(child, "expected expression");
            }
            // TODO: fail if no right child, or not a valid expression
            SyntaxNode *child_r = node->m_children[i + 1];
            node->m_children.erase(node->m_children.begin() + i + 1);
            child->append(child_r);
        }

        i--;
    }
}

void
Grammar::_postProcessBinary(std::set<std::string> &op,
                            SyntaxNode *node,
                            StringValue::Mode mode) {
    for (size_t i = 0; i < node->m_children.size();) {
        SyntaxNode *child = node->m_children[i];

        if (op.count(child->m_value.m_sValue) > 0) {

            child->m_value.m_mode = mode;

            bool has_lhs = i > 0;
            bool has_rhs = i + 1 < node->m_children.size();

            if (!has_lhs) {
                if (has_rhs && mode == StringValue::Mode::BinaryOperator) {
                    node->m_value.m_sValue += "???";
                    throw LHSError(child, node->m_children[i + 1],
                                   "expected column name");
                } else {
                    throw LHSError(child, "expected column name");
                }
            }
            SyntaxNode *child_l = node->m_children[i - 1];

            if (!has_rhs) {
                if (mode == StringValue::Mode::BinaryOperator)
                    throw RHSError(child, child_l, "expected value");
                else
                    throw RHSError(child, "expected expression");
            }

            SyntaxNode *child_r = node->m_children[i + 1];

            node->m_children.erase(node->m_children.begin() + i + 1);
            node->m_children.erase(node->m_children.begin() + i - 1);
            child->append(child_l);
            child->append(child_r);
        } else {
            i++;
        }
    }
}

std::unique_ptr<SearchRule>
Grammar::buildRule(SyntaxNode *node) {
    if (node->mode() == StringValue::Mode::FlowAnd ||
        node->mode() == StringValue::Mode::FlowParens ||
        node->mode() == StringValue::Mode::FlowOr) {
        SearchRule::SearchRuleList rules;
        for (SyntaxNode *nd : node->m_children) {
            std::unique_ptr<SearchRule> rule = buildRule(nd);
            if (rule)
                rules.push_back(std::move(rule));
        }
        // hoist the syntax rule when the joining operator is superfluous
        if (rules.size() == 1)
            return std::move(rules[0]);
        if (node->mode() == StringValue::Mode::FlowOr)
            return std::unique_ptr<SearchRule>(
                new OrSearchRule(std::move(rules)));
        if (node->mode() == StringValue::Mode::FlowAnd ||
            node->mode() == StringValue::Mode::FlowParens)
            return std::unique_ptr<SearchRule>(
                new AndSearchRule(std::move(rules)));
    } else if (node->mode() == StringValue::Mode::FlowNot) {
        // make the child node for logical not
        SyntaxNode *child = node->m_children.front();
        std::unique_ptr<SearchRule> rule = buildRule(child);
        return std::unique_ptr<SearchRule>(new NotSearchRule(std::move(rule)));
    } else if (node->mode() == StringValue::Mode::BinaryOperator) {
        SyntaxNode *left = node->m_children.front();
        SyntaxNode *right = node->m_children.back();
        return buildColumnRule(node, left, right);
    }
    // TODO error!! (should never happen)
    return nullptr;
}

SearchGrammar::SearchGrammar() : Grammar() {
    // define the characters used by the grammar. the tokenizer
    // will break the text into tokens of a similar class in a context free way.
    //util::time_point_t m_current_time;
    m_recalculate_time = true;

    m_sigil = ".";
    m_all_text = "all_text";
    m_char_classes[Grammar::CharacterClass::whitespace] = {
        ' ', '\t',
    };
    m_char_classes[Grammar::CharacterClass::base] = {
        '~', '!', '=', '<', '>',
    };
    // m_char_classes[Grammar::CharacterClass::control_flow] = { '|', '&' };
    m_char_classes[Grammar::CharacterClass::special] = {'~', '!', '=', '<',
                                                        '>', '|', '&'};
    // m_char_classes[Grammar::CharacterClass::negate] = { '!', };
    m_char_classes[Grammar::CharacterClass::nest_begin] = {
        '(',
    };
    m_char_classes[Grammar::CharacterClass::nest_end] = {
        ')',
    };
    m_char_classes[Grammar::CharacterClass::quote] = {
        '"',
    };
    m_char_classes[Grammar::CharacterClass::escape] = {
        '\\',
    };

    // opt_binary_operators
    m_flow_not = {"!"};
    m_flow_and = {"&&"};
    m_flow_or = {"||"};
    m_binary_operators = {"<", "<=", ">",  ">=", "=",
                          "~", "==", "=~", "!=", "!=="};

    for (const std::string &s : m_binary_operators) {
        m_all_operators.insert(s);
    }

    m_all_operators.insert("!");
    m_all_operators.insert("&&");
    m_all_operators.insert("||");

    m_operator_types["="] = SearchGrammar::RuleType::PartialString;
    m_operator_types["~"] = SearchGrammar::RuleType::PartialString;
    m_operator_types["!="] = SearchGrammar::RuleType::InvertedPartialString;
    m_operator_types["=="] = SearchGrammar::RuleType::Exact;
    m_operator_types["!=="] = SearchGrammar::RuleType::InvertedExact;
    m_operator_types["=~"] = SearchGrammar::RuleType::RegExpSearch;
    m_operator_types["<"] = SearchGrammar::RuleType::LessThan;
    m_operator_types["<="] = SearchGrammar::RuleType::LessThanEqual;
    m_operator_types[">"] = SearchGrammar::RuleType::GreaterThan;
    m_operator_types[">="] = SearchGrammar::RuleType::GreaterThanEqual;
}

std::string
SearchGrammar::getColumnName(SyntaxNode *nd_column) {
    std::string column = nd_column->text();

    // user has defined no shortcuts.
    if (m_col_abbreviations.empty())
        return column;

    // the input must match a shortcut
    if (m_col_abbreviations.count(column) > 0)
        return m_col_abbreviations[column];

    throw SyntaxError(nd_column, "unknown column " + nd_column->text());
}

std::unique_ptr<SearchRule>
SearchGrammar::buildColumnRule(SyntaxNode *nd_op,
                               SyntaxNode *nd_column,
                               SyntaxNode *nd_text) {
    // TODO: to handle syntax errors, I need to pass the nodes in,
    // not the text

    std::string op = nd_op->text();
    std::string column = getColumnName(nd_column);
    std::string text = nd_text->text();

    SearchGrammar::RuleType type = m_operator_types[op];

    // translate column : art -> artist
    //
    if (column == m_all_text) {
        return buildAllTextRule(type, text);
    }
    if (m_col_integer.count(column) > 0) {

        return buildIntegerColumnRule(type, column, nd_text);
    }
    if (m_col_date.count(column) > 0) {
        return buildDateColumnRule(type, column, nd_text);
    }
    return buildStringColumnRule(type, column, nd_text);
}

/**
 * for string-type columns '=' is interpreted as LIKE, apposed to
 * an exact match
 *
 */
std::unique_ptr<SearchRule>
SearchGrammar::buildStringColumnRule(SearchGrammar::RuleType type,
                                     const std::string &column,
                                     SyntaxNode *nd_text) {
    std::string text = nd_text->text();
    switch (type) {
    case SearchGrammar::RuleType::PartialString:
        return std::unique_ptr<SearchRule>(
            new PartialStringSearchRule(column, text));
    case SearchGrammar::RuleType::InvertedPartialString:
        return std::unique_ptr<SearchRule>(
            new InvertedPartialStringSearchRule(column, text));
    case SearchGrammar::RuleType::Exact:
        return std::unique_ptr<SearchRule>(
            new ExactSearchRule<std::string>(column, text));
    case SearchGrammar::RuleType::InvertedExact:
        return std::unique_ptr<SearchRule>(
            new InvertedExactSearchRule<std::string>(column, text));
    case SearchGrammar::RuleType::RegExpSearch:
        throw SyntaxError(nd_text, "REGEX not yet supported on string columns");
    case SearchGrammar::RuleType::LessThan:
        return std::unique_ptr<SearchRule>(
            new LessThanSearchRule<std::string>(column, text));
    case SearchGrammar::RuleType::LessThanEqual:
        return std::unique_ptr<SearchRule>(
            new LessThanEqualSearchRule<std::string>(column, text));
    case SearchGrammar::RuleType::GreaterThan:
        return std::unique_ptr<SearchRule>(
            new GreaterThanSearchRule<std::string>(column, text));
    case SearchGrammar::RuleType::GreaterThanEqual:
        return std::unique_ptr<SearchRule>(
            new GreaterThanEqualSearchRule<std::string>(column, text));
    default:
        throw ParseError("Unspecified Error");
    }
}

std::unique_ptr<SearchRule>
SearchGrammar::buildIntegerColumnRule(SearchGrammar::RuleType type,
                                      const std::string &column,
                                      SyntaxNode *nd_text) {
    // there is no reason to parse this and pass it on as an integer,
    // when the node will just convert it back to a string when
    // formatting the sql.
    // However this sets us up for potential base conversions in the future...

    int value = util::fromString<int>(nd_text->text());
    switch (type) {
    case SearchGrammar::RuleType::PartialString:
    case SearchGrammar::RuleType::Exact:
        return std::unique_ptr<SearchRule>(
            new ExactSearchRule<int>(column, value));
    case SearchGrammar::RuleType::InvertedPartialString:
    case SearchGrammar::RuleType::InvertedExact:
        return std::unique_ptr<SearchRule>(
            new InvertedExactSearchRule<int>(column, value));
    case SearchGrammar::RuleType::LessThan:
        return std::unique_ptr<SearchRule>(
            new LessThanSearchRule<int>(column, value));
    case SearchGrammar::RuleType::LessThanEqual:
        return std::unique_ptr<SearchRule>(
            new LessThanEqualSearchRule<int>(column, value));
    case SearchGrammar::RuleType::GreaterThan:
        return std::unique_ptr<SearchRule>(
            new GreaterThanSearchRule<int>(column, value));
    case SearchGrammar::RuleType::GreaterThanEqual:
        return std::unique_ptr<SearchRule>(
            new GreaterThanEqualSearchRule<int>(column, value));
    case SearchGrammar::RuleType::RegExpSearch:
        throw SyntaxError(nd_text,
                          "cannot perform REGEX search on integer columns");
    default:
        throw ParseError("Unspecified Error");
    }
}

std::unique_ptr<SearchRule>
SearchGrammar::buildDateColumnRule(SearchGrammar::RuleType type,
                                      const std::string &column,
                                      SyntaxNode *nd_text) {

    // draft specificatoin for parser

    // |<a   2   b>| |<c   1   d>| |<e 0 f>|
    // [ 2days ago ] [ yesterday ] [ today ]

    // y,m,d := todays date
    // =0d  := EPOCHTIME(y,m,d)   < x < EPOCHTIME(y,m,d+1)
    // =1d  := EPOCHTIME(y,m,d-1) < x < EPOCHTIME(y,m,d)
    // <0d  := x > EPOCHTIME(y,m,d)
    // >0d  := x < EPOCHTIME(y,m,d)
    // <1d  := x > EPOCHTIME(y,m,d-1)
    // >1d  := x < EPOCHTIME(y,m,d-1)
    // <= := <
    // >= := >

    // = 2000/1/1 := EPOCHTIME(2000,1,1) < x < EPOCHTIME(2000,1,2)
    // > 2000/1/1 := x > EPOCHTIME(2000,1,1)
    // < 2000/1/1 := x < EPOCHTIME(2000,1,1)
    // <= := <
    // >= := >

    // =2016 -> add1 unit should be year
    // =2016/m -> add1 unit should be month

    int value=0, value_lo=0, value_hi=0;

    if (m_recalculate_time) {
        m_current_time = util::currentTimePoint();
    }

    util::ydate_t date = util::extractDate(m_current_time);

    util::parseRelativeDateDelta(nd_text->text(),date,true);

    switch (type) {
    case SearchGrammar::RuleType::PartialString:
    case SearchGrammar::RuleType::Exact:
        value_lo = util::dateToEpochTime(date);
        date = util::dateDelta(date,0,0,1);
        value_hi = util::dateToEpochTime(date);
        return std::unique_ptr<SearchRule>(
            new RangeSearchRule<int>(column, value_lo, value_hi));
    case SearchGrammar::RuleType::InvertedPartialString:
    case SearchGrammar::RuleType::InvertedExact:
        value_lo = util::dateToEpochTime(date);
        date = util::dateDelta(date,0,0,1);
        value_hi = util::dateToEpochTime(date);
        return std::unique_ptr<SearchRule>(
            new NotRangeSearchRule<int>(column, value_lo, value_hi));
    case SearchGrammar::RuleType::LessThan:
    case SearchGrammar::RuleType::LessThanEqual:
        value = util::dateToEpochTime(date);
        return std::unique_ptr<SearchRule>(
            new GreaterThanEqualSearchRule<int>(column, value));
    case SearchGrammar::RuleType::GreaterThan:
    case SearchGrammar::RuleType::GreaterThanEqual:
        value = util::dateToEpochTime(date);
        return std::unique_ptr<SearchRule>(
            new LessThanEqualSearchRule<int>(column, value));
    case SearchGrammar::RuleType::RegExpSearch:
        throw SyntaxError(nd_text,
                          "cannot perform REGEX search on date columns");
    default:
        throw ParseError("Unspecified Error");
    }

}
std::unique_ptr<SearchRule>
SearchGrammar::buildAllTextRule(SearchGrammar::RuleType type,
                                const std::string &text) {

#define newAllTextRule(R, text)                                 \
    std::unique_ptr<SearchRule>(                                \
        new MultiColumnSearchRule<R<std::string>, std::string>( \
            m_col_text, text, m_all_text))

    switch (type) {
    case SearchGrammar::RuleType::PartialString:
        return std::unique_ptr<SearchRule>(
            new MultiColumnSearchRule<PartialStringSearchRule, std::string>(
                m_col_text, text, m_all_text));
    case SearchGrammar::RuleType::InvertedPartialString:
        return std::unique_ptr<SearchRule>(
            new MultiColumnSearchRule<InvertedPartialStringSearchRule,
                                      std::string>(m_col_text, text,
                                                   m_all_text));
    case SearchGrammar::RuleType::Exact:
        return newAllTextRule(ExactSearchRule, text);
    case SearchGrammar::RuleType::InvertedExact:
        return newAllTextRule(InvertedExactSearchRule, text);
    case SearchGrammar::RuleType::RegExpSearch:
        return nullptr; // todo
    case SearchGrammar::RuleType::LessThan:
        return newAllTextRule(LessThanSearchRule, text);
    case SearchGrammar::RuleType::LessThanEqual:
        return newAllTextRule(LessThanEqualSearchRule, text);
    case SearchGrammar::RuleType::GreaterThan:
        return newAllTextRule(GreaterThanSearchRule, text);
    case SearchGrammar::RuleType::GreaterThanEqual:
        return newAllTextRule(GreaterThanEqualSearchRule, text);
    default:
        return nullptr; // todo error!
    }
    return std::unique_ptr<SearchRule>(nullptr);
}

} /*namespace core*/
} /*namespace yue*/
