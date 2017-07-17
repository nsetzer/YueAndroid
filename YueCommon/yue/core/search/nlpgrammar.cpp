
#include "yue/core/search/nlpgrammar.hpp"
#include "yue/core/util/string.h"

#include <map>
#include <utility> // pair
#include <vector>
#include <string>

namespace yue {
namespace core {

namespace {

void write_cfg(std::ostream& out, cfg_t& cfg) {
    for (auto& kv : cfg) {
        int counter = 1;
        const std::string& term = kv.first;
        std::vector<weightedproduction_t>& productions = kv.second;

        for( weightedproduction_t& wprod : productions ) {

            double weight = wprod.first;
            std::vector<std::string>& prod = wprod.second;

            out << weight << " " << term << " ->";

            for (size_t i=0; i < prod.size(); i++)
                out << " " << prod[i];

            out << std::endl;
        }
    }
}

} // anonymous namespace


CYKParser::CYKParser()
    : m_cfg()
    , m_cnf()
    , m_lutnt()
    , m_lutt()
{
}

void CYKParser::load_cfg(std::istream& stream)
{

    std::string line;

    while (stream.good()) {
        std::getline(stream,line);

        util::trim(line);

        if (util::startswith(line,"#") || line.size() == 0) {
            continue;
        }


        std::vector<std::string> items = util::split(line, ' ');

        if (items.size() >= 3) {

            std::string term = items[1];
            double weight = 1; // TODO parse weight

            items.erase(items.begin()); // remove weight
            items.erase(items.begin()); // remove term

            if (m_cfg.count(term) == 0) {
                m_cfg[term] = std::vector<weightedproduction_t>();
            }

            m_cfg[term].push_back({weight,items});

        } else {
            std::cout << "poorly  formed input line: " << line << std::endl;
        }

    }

    size_t nrules=0;
    for (auto& kv : m_cfg) {
        int counter = 1;
        const std::string& term = kv.first;
        std::vector<weightedproduction_t>& productions = kv.second;
        nrules += productions.size();
    }
    std::cout << "loaded " << nrules << " cfg rules" << std::endl;

    convert_cfg_tocnf();

    invert_cnf();

}

void CYKParser::convert_cfg_tocnf()
{
    // 5 steps to voncert CFG to CNF : START,TERM,BIN,DEL,UNIT
    // START and DEL are not implemented

    for (auto& kv : m_cfg) {
        int counter = 1;
        const std::string& term = kv.first;
        std::vector<weightedproduction_t>& productions = kv.second;

        for( weightedproduction_t& wprod : productions ) {

            double weight = wprod.first;
            std::vector<std::string> prod = wprod.second;

            if (prod.size() > 1) {

                // TERM: replace terminals with non terminals
                for (size_t i=0; i < prod.size(); i++) {
                    // if the production is Terminal, replace
                    // it with a nonterminal
                    if (m_cfg.count(prod[i])==0) {
                        std::string temp = "<T:" + prod[i] + ">";
                        add_cnf(temp, {weight, {prod[i],}});
                        prod[i] = temp;
                    }

                }

                // BIN: binarize productions
                while (prod.size() > 2) {
                    std::stringstream ss;
                    ss << "<B:" << term << "." << counter << ">";
                    counter ++;
                    std::string temp = ss.str();

                    size_t j = prod.size()-1;
                    size_t i = j-1;
                    add_cnf(temp, {weight, {prod[i],prod[j]}});

                    prod.pop_back();
                    prod.pop_back();
                    prod.push_back( temp );
                }

                // add a rule of the form A -> B C
                add_cnf(term, {weight, prod});

            } else {
                // rule is either A->B or A->x
                add_cnf(term, wprod);
            }
        }

    }

    {
        size_t nrules=0;
        for (auto& kv : m_cnf) {
            int counter = 1;
            const std::string& term = kv.first;
            std::vector<weightedproduction_t>& productions = kv.second;
            nrules += productions.size();
        }
        std::cout << "loaded " << nrules << " cnf rules" << std::endl;
    }

    // UNIT: expand rules of the form A -> B to A -> x
    // this can be done safely because at this point, all rules
    // are either A -> B C, A -> x, or A -> B (the ones to fix)

    for (auto& kv : m_cnf) {
        int counter = 1;
        const std::string& term = kv.first;
        std::vector<weightedproduction_t>& productions = kv.second;

        size_t i=0;
        while (i < productions.size()) {
            // if the rule produces a single non terminal output
            // remove the rule, and add the productions from following
            // the expansion of that rule.
            if (productions[i].second.size()==1 &&
                m_cnf.count(productions[i].second[0])>0) {

                std::string rule = productions[i].second[0];
                productions.erase(productions.begin()+i);
                for (weightedproduction_t& wprod : m_cnf[rule]) {
                    productions.push_back(wprod);
                }
            } else {
                i += 1;
            }
        }
    }

    size_t nrules=0;
    for (auto& kv : m_cnf) {
        int counter = 1;
        const std::string& term = kv.first;
        std::vector<weightedproduction_t>& productions = kv.second;
        nrules += productions.size();
    }
    std::cout << "loaded " << nrules << " cnf rules" << std::endl;

    //write_cfg(std::cout, m_cnf);

}

// force copy of wprod
void CYKParser::add_cnf(const std::string& key, weightedproduction_t prod)
{
    if (m_cnf.count(key) == 0) {
        m_cnf[key] = std::vector<weightedproduction_t>();
    }

    std::vector<std::string>& prodb = prod.second;

    for( weightedproduction_t& wprod : m_cnf[key] ) {
        double weight = wprod.first;
        std::vector<std::string>& proda = wprod.second;

        // check to see if the production exists in the map already
        // only compare the terms, not the weights.
        // the length needs to be the same, and all strings must be equal.
        if (proda.size() != prodb.size())
            continue;

        bool equal = true;
        for (size_t i=0; i < prodb.size(); i++) {
            if (proda[i] != prodb[i]) {
                equal = false;
                break;
            }
        }

        if (equal) {
            // found prod in the cnf under the given key, return and do nothing.
            return;
        }
    }

    m_cnf[key].push_back( prod );
}

void CYKParser::invert_cnf()
{

    for (auto& kv : m_cnf) {
        int counter = 1;
        const std::string& term = kv.first;
        std::vector<weightedproduction_t>& productions = kv.second;

        for( weightedproduction_t& wprod : productions ) {

            double weight = wprod.first;
            std::vector<std::string>& prod = wprod.second;

            if (term == "VALUE") {
                weight = 1000;
            } else {
                weight = 1;
            }

            if (prod.size() == 2) {
                rcnfnt_i index{prod[0],prod[1]};

                //std::cout << "invert " << weight << " " << prod[0] << "," << prod[1] << " -> " << term << std::endl;
                if (m_lutnt.count(index)==0) {
                    m_lutnt[index] = {{weight,term},};
                } else {
                    m_lutnt[index].push_back( {weight,term} );
                }
            } else {
                std::string index = prod[0];
                //std::cout << "invert " << weight << " " << prod[0] << " -> " << term << std::endl;
                if (m_lutt.count(index)==0) {
                    m_lutt[index] = {{weight,term},};
                } else {
                    m_lutt[index].push_back( {weight,term} );
                }
            }
        }
    }
}

NLPContext::NLPContext()
{


}

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

void CYKParser::parse(std::vector<std::string> sentence) {
    // TODO: convert to use interned strings for fast comparisons
    //       and merge lutt/luttn
    // TODO: table entries are unique by TERM, with minimum weight.

    table_t table;

    int N = sentence.size();

    // hand written reverse CNF grammar
    //lutt["artist"] = {{0,"COLUMN"},};
    //lutt["is"] = {{0,"OPERATION"},};
    //lutt["OOV"] = {{0,"TEXT"},};
    //lutt["."] = {{0,"<.>"},};
    //lutnt[{"COLUMN","OPERATION"}] = {{0,"SEARCHTERM.1"},};
    //lutnt[{"SEARCHTERM.1","TEXT"}] = {{0,"SEARCHTERM"},};
    //lutnt[{"SEARCHTERM","<.>"}] = {{0,"START"},};

    for (int j=1; j <= N; j++ ) {
        //std::cout << "j:" << j << std::endl;
        std::pair<int,int> idx_j(j-1,j);
        table[idx_j] = std::vector<TableEntry>();
        for ( weightedterm_t wt : m_lutt[sentence[j-1]]) {
            std::cout << idx_j.first << ","
                      << idx_j.second << ": "
                      << wt.second << std::endl;
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
                        if (m_lutnt.count(idx_r) > 0) {
                            for (weightedterm_t wt : m_lutnt[idx_r]) {
                                std::cout << idx_a.first << ","
                                          << idx_a.second << ": "
                                          << wt.second << std::endl;

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

    bool match = false;
    std::pair<int,int> idx_a(0,N);
    for (TableEntry& ent : table[idx_a]) {
        if (ent.term == "START") {
            std::cout << "START(";
            print_tree(sentence,table,idx_a,0);
            std::cout << ")" << std::endl;
            match = true;
        }
    }

    std::cout << "Match: " << ((match)?"true":"false") << std::endl;
}

} // namespace core
} // namespace yue

