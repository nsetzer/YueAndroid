
#include "yue/core/search/nlpgrammar.hpp"

#include <map>
#include <utility> // pair
#include <vector>
#include <string>

namespace yue {
namespace core {

NLPContext::NLPContext()
{

}

// an index into the CYK table
typedef std::pair<int,int> table_i;

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

// the table holding possibly parse states
typedef std::map<table_i,std::vector<TableEntry>> table_t;
// for a rule, either A -> B C or A -> x, this type stores the
// weight for following the rule A.
typedef std::pair<double,std::string> weightedterm_t;
// the reverse grammar is made up of two maps
// nonterminal rules A -> B C  use a map taking a pair of names from the rhs
// terminal rules A -> x use a map taking the name of the rhs.
typedef std::pair<std::string,std::string> rcnfnt_i;
typedef std::map<rcnfnt_i,std::vector<weightedterm_t>> rcnfnt_t;
typedef std::map<std::string,std::vector<weightedterm_t>> rcnft_t;

void print_tree(std::vector<std::string>& sentence, table_t& table, table_i idx, int k) {

    TableEntry& ent = table[idx][k];
    if (ent.idx_t >=0) {
        std::cout << ent.term << "(" << sentence[ent.idx_t] << ")";
    } else {
        std::cout << ent.term << "(";
        print_tree(sentence,table, ent.idx_b,ent.idx_bk);
        std::cout << ", ";
        print_tree(sentence,table, ent.idx_c,ent.idx_ck);
        std::cout << ")";
    }

}

void table_append(table_t& table, std::pair<int,int> idx_a, TableEntry ent) {
    for (int ti=0; ti < table[idx_a].size(); ti++) {
        // replace if the term is the same and the weight is lower.
        if (table[idx_a][ti].term == ent.term) {
            if (ent.weight < table[idx_a][ti].weight) {
                table[idx_a][ti] = ent;
            }
            return;
        }
    }

    table[idx_a].push_back( ent );
    return;
}
void cykparse(std::vector<std::string> sentence) {
    // TODO: convert to use interned strings for fast comparisons
    //       and merge lutt/luttn
    // TODO: table entries are unique by TERM, with minimum weight.

    table_t table;
    rcnfnt_t lutnt; // nonterminal lookup table
    rcnft_t lutt; // terminal lookup table

    int N = sentence.size();

    // hand written reverse CNF grammar
    lutt["artist"] = {{0,"COLUMN"},};
    lutt["is"] = {{0,"OPERATION"},};
    lutt["audio"] = {{0,"TEXT"},};
    lutt["."] = {{0,"<.>"},};
    lutnt[{"COLUMN","OPERATION"}] = {{0,"SEARCHTERM.1"},};
    lutnt[{"SEARCHTERM.1","TEXT"}] = {{0,"SEARCHTERM"},};
    lutnt[{"SEARCHTERM","<.>"}] = {{0,"START"},};

    for (int j=1; j <= N; j++ ) {
        //std::cout << "j:" << j << std::endl;
        std::pair<int,int> idx_j(j-1,j);
        table[idx_j] = std::vector<TableEntry>();
        for ( weightedterm_t wt : lutt[sentence[j-1]]) {
            //std::cout << idx_j.first << ","
            //          << idx_j.second << ": "
            //          << wt.second << std::endl;
            table[idx_j].push_back(TableEntry(wt.second,wt.first,j-1));
        }
        for (int i=j-2; i>=0; i--) {
            for (int k=i+1; k<j; k++) {
                std::pair<int,int> idx_a(i,j);
                std::pair<int,int> idx_b(i,k);
                std::pair<int,int> idx_c(k,j);
                std::vector<TableEntry>& lst_b = table[idx_b];
                std::vector<TableEntry>& lst_c = table[idx_c];
                int len_b = lst_b.size();
                int len_c = lst_c.size();
                if (table.count(idx_a)==0) {
                    table[idx_a] = std::vector<TableEntry>();
                }
                for (int xi=0; xi < len_b; xi++) {
                    for (int xj=0; xj < len_c; xj++) {
                        TableEntry& b = lst_b[xi];
                        TableEntry& c = lst_c[xj];
                        rcnfnt_i idx_r(b.term,c.term);
                        if (lutnt.count(idx_r) > 0) {
                            for (weightedterm_t wt : lutnt[idx_r]) {
                                //std::cout << idx_a.first << ","
                                //          << idx_a.second << ": "
                                //          << wt.second << std::endl;

                                table_append(table,idx_a,TableEntry(
                                    wt.second,
                                    wt.first + b.weight + c.weight,
                                    idx_b,xi,
                                    idx_c,xj ));
                                //table[idx_a].push_back(TableEntry(
                                //    wt.second,
                                //    wt.first + b.weight + c.weight,
                                //    idx_b,xi,
                                //    idx_c,xj ));
                            }
                        }
                    }
                }
            }
        }
    }
    std::pair<int,int> idx_a(0,N);
    for (TableEntry& ent : table[idx_a]) {
        if (ent.term == "START") {
            print_tree(sentence,table,idx_a,0);
            std::cout << std::endl;
        }
    }
}

} // namespace core
} // namespace yue

