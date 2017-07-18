
#ifndef YUE_CORE_SEARCH_NLPGRAMMAR_HPP
#define YUE_CORE_SEARCH_NLPGRAMMAR_HPP

#include "yue/core/util/dateutil.h"
#include "yue/core/search/rules.hpp"
#include "yue/core/search/grammar.hpp"

#include <istream>
#include <fstream>

namespace yue {
namespace core {

class TableEntry;



// an index into the CYK table
typedef std::pair<int,int> table_i;
// the table holding possibly parse states
typedef std::map<table_i,std::vector<TableEntry>> table_t;
// a weighted production rule, right hand side of A -> B C or A -> x
typedef std::pair<double,std::vector<std::string>> weightedproduction_t;
// for a rule, either A -> B C or A -> x, this type stores the
// weight for following the rule A.
typedef std::pair<double,std::string> weightedterm_t;
// the reverse grammar is made up of two maps
// nonterminal rules A -> B C  use a map taking a pair of names from the rhs
// terminal rules A -> x use a map taking the name of the rhs.
typedef std::pair<std::string,std::string> rcnfnt_i;
typedef std::map<rcnfnt_i,std::vector<weightedterm_t>> rcnfnt_t;
typedef std::map<std::string,std::vector<weightedterm_t>> rcnft_t;

typedef std::map<std::string,std::vector<weightedproduction_t>> cfg_t;

class TableEntry
{
public:
    // table entry for a nonterminal node (idx_t has no meaning)
    // idx_b,idx_c point to indices in the table that generated this entry
    TableEntry(std::string term_, double weight_,
               table_i idx_b_, int bk,
               table_i idx_c_, int ck)
        : term(term_)
        , weight(weight_)
        , idx_b(idx_b_)
        , idx_bk(bk)
        , idx_c(idx_c_)
        , idx_ck(ck)
        , idx_t(-1) // default invalid
    {}
    // table entry for a terminal node (idx_t has meaning)
    TableEntry(std::string term_, double weight_, int idx_)
        : term(term_)
        , weight(weight_)
        , idx_t(idx_)
    {}
    ~TableEntry() {}

    std::string term;
    double weight;

    int idx_t;

    table_i idx_b;
    table_i idx_c;

    int idx_bk;
    int idx_ck;

};



class CYKParser
{

public:
    CYKParser();
    ~CYKParser() = default;

    void load_cfg(std::istream& stream);

    void parse(std::vector<SyntaxNode*>& sentence);

private:

    void convert_cfg_tocnf();
    void add_cnf(const std::string& key, weightedproduction_t prod);
    void invert_cnf();

    cfg_t m_cfg; // context free grammar
    cfg_t m_cnf; // m_cfg in chomsky normal form
    rcnfnt_t m_lutnt; // reverse m_cnf, for non terminals
    rcnft_t m_lutt;   // reverse m_cnf, for terminals
};

class NLPContext
{
public:
    NLPContext();
    ~NLPContext() = default;
};



class NLPSearchGrammar : public SearchGrammar
{

    CYKParser m_parser;

public:
    NLPSearchGrammar();
    ~NLPSearchGrammar() = default;

    //virtual void tokenize(std::string text);
    virtual void postProcess(SyntaxNode *root);

    void test(std::string s) {
        tokenize(s);
        postProcess(tokens());
    }

};



} // namespace core
} // namespace yue

#endif // YUE_CORE_SEARCH_NLPGRAMMAR_HPP