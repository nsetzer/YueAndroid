#!/bin/python $this

import sys
from collections import defaultdict
from collections import namedtuple
from itertools import product
import random
import bisect
"""
todo remember that trees can be built out of vectors by
remembering the index in the vector instead of a pointer.

"""

def format_tree(tree,tab=0,tab_width=3,width=80):
    """
        a tree is a list-of-values
        tree:
            [ string ]         a leaf node, with label "string"
            [ string child ... ] a node with N child trees.
        child: tree | string

    """
    # this degenerates to the print_tree_old function
    # when the width is too small

    # if the tree is a terminal node, just format that element
    if isinstance(tree,str):
        return ["'%s'"%tree,]

    lines = []
    for subtree in tree[1:]:
        lines += format_tree(subtree, tab+1, tab_width, width)

    available = width - tab*tab_width

    # +5 for the four extra characters used for single line formating
    total = sum(map(len,lines)) + len(tree[0]) + 5

    # format everything on a single line
    if total < available:
        return ["%s ( "%tree[0] + ' '.join(lines) + " )",]

    # format everything as multiple lines
    # first add the tab character to every line
    s = " "*(tab_width)
    for i in range(len(lines)):
        lines[i] = s+lines[i]
    # format header and footer
    lines.insert(0,"%s ( "%tree[0])
    lines.append(")")

    return lines

def print_tree(tree,tab_width=3,width=80):
    for line in format_tree(tree,tab_width=tab_width,width=width-1):
        sys.stdout.write(line+"\n")

def read_grammar(path):

    nonterm = defaultdict(list)

    with open(path) as rf:
        for line in rf:
            line = line.strip();
            if not line or line.startswith('#'):
                continue

            terms = line.split()

            nonterm[ terms[1] ].append( (int(terms[0]),terms[2:]) )

    return nonterm

def isTerm(grammar,key):
    return key not in grammar

def isCNF(grammar, prod):
    """ return true if a given production is in CNF form """
    p,seq = prod
    l = len(seq)
    if l == 1:
        return isTerm(grammar,seq[0])
    if l == 2:
        return not isTerm(grammar,seq[0]) and not isTerm(grammar,seq[1])
    return False;

def print_grammar(grammar):
    print("---")
    fmtlen = 1+max(len(k) for k in grammar.keys())
    fmt = "%%-%ds: %%s"%(fmtlen)
    for key,prod in sorted(grammar.items()):
        items = ["%.2f:%s"%(x[0],' '.join(x[1])) for x in prod]
        msg = fmt%(key,items[0])
        dirty = True;
        for item in items[1:]:
            if 2 + len(item) + len(msg) > 79:
                print(msg)
                dirty = False
                msg = fmt%("",item)
            else:
                msg = msg + ", " + item
                dirty = True
        if dirty:
            print(msg)
    print("...")

def CFGtoCNF(grammar):

    # START,TERM,BIN,DEL,UNIT

    cnf = defaultdict(list)

    print_grammar(grammar)

    # START: removing START on RHS not implemented

    def append(key,w,prod):
        for _, prod2 in cnf[key]:
            if prod == prod2:
                return
        cnf[key].append( (w,prod) )

    # TERM, BIN
    for key,wprods in grammar.items():
        counter=1

        for (w,prod) in wprods:

            if len(prod) > 1:
                # TERM: replace terminals with non terminals
                for i in range(len(prod)):
                    t = prod[i]
                    if isTerm(grammar,t):
                        t2 = "<T:%s>"%t
                        prod[i] = t2
                        append(t2, w, [t,])

                # BIN: binaraize productions
                while len(prod) > 2:
                    tmp = prod[-2:]
                    t = "<B:%s.%d>"%(key,counter)
                    counter += 1
                    append(t,w,tmp)
                    prod = prod[:-2] + [t,]
                append(key, w, prod)

            else:
                # if this is a rule A ->B, it will be fixed in the UNIT step
                append(key, w, prod)

    # DEL: remove epsilon productions not implemented

    nrules = 0
    for rules in cnf.values():
        nrules += len(rules)
    print("loaded %d cnf rules"%nrules)
    # UNIT

    for key,wprods in cnf.items():
        i=0;
        while i < len(wprods):
            (w,prod) = wprods[i]
            if len(prod)==1 and not isTerm(cnf,prod[0]):
                wprods.pop(i)
                wprods += cnf[prod[0]]
            else:
                i += 1

    # deduplicate

    # renormalize

    #for key in cnf.keys():
    #    wprods = cnf[key]
    #    totalw = sum( w for w,p in wprods )
    #    cnf[key] = [ (w/totalw,p) for w,p in wprods ]

    # ---

    print_grammar(cnf)

    return cnf

def InvertCNF(grammar):

    # for each terminal, determine the set of non
    # terminals which produce it.
    icnf = defaultdict(list)
    for key,prods in sorted(grammar.items()):
        for prod in prods:
            w,seq = prod
            if len(seq) == 1:
                # terminal unary rules
                icnf[seq[0]].append( (w,key) )
            else:
                # nonterminal binary rules
                icnf[tuple(seq)].append( (w,key) )
    return icnf;

def CKYParser(grammar,sentence):
    """
    grammar must be in CNF form

    keep pointers for each non terminal
        pointers are the indices of the table that produced that entry

    """



    N  = len(sentence)

    lutCNF = InvertCNF(grammar)
    sentence = [ w if w in lutCNF else "OOV" for w in sentence  ]

    print("sentence:", ' '.join(sentence))

    table = defaultdict(list)

    # each table entry is a set, with unique terms.
    # keep only the term that minimizes the weight
    def append(idx, term, weight, producers):
        tableitems = table[idx];
        for i in range(len(tableitems)):
            t,w,p = tableitems[i]
            if t == term:
                if weight < w:
                    tableitems[i] = (term,weight,producers)
                return;
        table[idx].append( (term, weight, producers) )
        return

    for j in range(1,N+1):
        table[(j-1,j)] = list([ (x, w, j-1) for w,x in lutCNF[sentence[j-1]] ])
        #table[(j-1,j)].append( ("SEARCHTERM", 1000, j-1) )
        #table[(j-1,j)].append( ("VALUE", 1000, j-1) )
        #table[(j-1,j)].append( ("<*>", 1000, j-1) )
        for i in reversed(range(0,j-1)):
            for k in range(i+1,j):
                for ix in range(len(table[(i,k)])):
                    for jx in range(len(table[(k,j)])):
                        # extract the rule and weight and drop the parent pointer
                        b,wb,_ = table[(i,k)][ix]
                        c,wc,_ = table[(k,j)][jx]
                        if (b,c) in lutCNF:
                            for (wt,t) in lutCNF[(b,c)]:
                                # store the rule, weight #
                                # and index that produced this rule
                                item = (t, wt+wb+wc, ((i,k,ix),(k,j,jx)))
                                #if item not in table[(i,j)]:
                                #    #table[(i,j)].append(item)
                                append((i,j), *item)

    return table

# these next few functions build and rebalance trees from the cyk results

def build_tree(sentence, table, state ):
    i,j,k = state
    r = table[(i,j)]
    if len(r)==0:
        return []
    a,w,d= table[(i,j)][k]
    if isinstance(d,int):
        tree = [a, sentence[d]]
    else:
        b,c = d
        tree = [a, build_tree(sentence, table, b), build_tree(sentence, table, c)]
    return tree

def build_tree2(sentence, table, state ):
    i,j,k = state
    r = table[(i,j)]
    if len(r)==0:
        return []
    a,w,d= table[(i,j)][k]
    if isinstance(d,int):
        tree = [sentence[d],]
        #if a in ("VALUE","<*>"):
        #    return sentence[d]
    else:
        b,c = d
        tree = []
        b = build_tree2(sentence, table, b)
        c = build_tree2(sentence, table, c)
        # remove hidden labels (ones added by the grammar automatically)
        # hoist like values

        _textlbls = ("VALUE","TEXT")
        if b[0] in _textlbls and c[0] in _textlbls:
            tree = b[1:] + c[1:]
            a = "TEXT" # not sure if required?
        else:
            if a in _textlbls and b[0] in _textlbls or b[0].startswith("<"):
                tree = b[1:]
            else:
                tree.append(b)

            if a in _textlbls and c[0] in _textlbls or c[0].startswith("<"):
                tree +=  c[1:]
            else:
                tree.append(c)

    if "SEARCHTERM" in a and not a.startswith("<"):
        a = "SEARCHTERM"

    tree.insert(0,a)

    return tree


def extract_expr(tree):
    if not isinstance(tree,list):
        return [tree,]
    res = []
    for child in tree[1:]:
        res += extract_expr(child)
    return res

def rebalance_tree(tree):

    i=0;
    while i < len(tree) :
        child = tree[i]
        if child[0].startswith("<") or child[0].startswith("SEARCHTERM"):
            tree.pop(i)
            for j, item in enumerate(child[1:]):
                tree.insert(i+j,item)
        else:
            i+= 1


    for child in tree[1:]:
        if isinstance(child,list):
            rebalance_tree(child)

    if tree[0] == "VALUE":
        tree[0] = "TEXT"

    i=1;
    while i < len(tree):

        child = tree[i]

        # combine value and text into a single node, TEXT

        if tree[0] == "COLUMN":

            expr =' '.join(tree[1:])
            print(len(tree))
            while len(tree) > 1:
                tree.pop()
            tree.append(expr)
            i += 1

        elif tree[0] == "TEXT" and child[0] == "TEXT":
            tree.pop(i)
            for item in child[1:]:
                tree.insert(i,item)
                i+= 1

        # combine ternary operations into binary nodes with the
        # operation as the root
        elif child[0] in ("OPERATION","BINOPERATION"):

            expr = ' '.join(extract_expr(child)) # child.pop(1)
            if expr == "greater than":
                expr = ">"
            elif expr == "less than":
                expr = "<"
            elif expr in ("equals","matches","is"):
                expr = "="
            elif expr == "is not":
                expr = "!="
            child[0] = expr
            while len(child) > 1:
                child.pop()
            rhs = tree.pop(i+1)
            lhs = tree.pop(i-1)
            child.append(lhs)
            child.append(rhs)

        elif child[0] == "DATEOPERATION":
            rhs = tree.pop(i+1)
            rhs = ' '.join(extract_expr(rhs))
            tree[i] = ["<", "date", rhs]

        else:
            i+=1


    for terms in ( ("and","&&") , ("or","||")):
        i=len(tree)-1;
        while i >=1 :
            child = tree[i]
            if child[0] == "JOIN" and child[1] in terms:
                child[0] = child.pop(1)
                rhs = tree.pop(i+1)
                lhs = tree.pop(i-1)
                child.append(lhs)
                child.append(rhs)
            i-= 1



def print_table(N,table):
    for i in range(N+1):
        for j in range(N+1):
            items = table[(i,j)]
            if len(items)>0:
                sys.stdout.write(items[0][0] + " ")
            else:
                sys.stdout.write(". ")
        sys.stdout.write("\n ")

def CKYRecognizer(grammar,sentence,start):
    """
    grammar must be in CNF form
    """

    N  = len(sentence)

    table = CKYParser(grammar,sentence)

    #print_table(N,table);

    index = None
    weight = 2**31

    for k,(token,w,_) in enumerate(table[(0,N)]):
         # if recognition terminated.
        if token == start:
            # find the minimum weight
            print("weight: ", w, weight, k)
            if (w < weight):
                weight = w;
                index = k
    #print("weight: ", weight)
    if index != None:
        return table, (0,N,index)
    return table, None

def random_choice(items,r=None):
    """ where items is a list of 2-tuple (weight,object)
        the sum of all weights in items should sum to one
    """
    wsum=0.0

    if r is None:
        r = random.random()

    for (w,x) in items:
        wsum += w
        if r <= wsum:
            return x
    return items[-1][1]

def generate(grammar,start="START"):

    # expand the starting rule...
    sentence = random_choice(grammar[start])

    # replace non-terminals....
    idx=0;

    while idx < len(sentence):
        t = sentence[idx];
        if t in grammar:
            sentence = sentence[:idx] + random_choice(grammar[t]) + sentence[idx+1:]
        else:
            idx+=1;

    # return a list of terminal strings
    return sentence

def main():

    grammar_path = "grammar.txt"
    grammar = read_grammar(grammar_path)

    nrules = 0
    for rules in grammar.values():
        nrules += len(rules)
    print("loaded %d cfg rules"%nrules)

    # penalize OOV words when matching.
    grammar_p = grammar.copy()
    grammar_p["TEXT"] = [(1000,p) for w,p in grammar_p["TEXT"]]
    grammar_p["VALUE"] = [(1000,p) for w,p in grammar_p["VALUE"]]
    cnf = CFGtoCNF(grammar_p)

    nrules = 0
    for rules in cnf.values():
        nrules += len(rules)
    print("loaded %d cnf rules"%nrules)

    for key,prod in sorted(cnf.items()):
        print(key,prod)

    #sentence = "artist is blah and album is blah or ( play count < 5 ) .".split()
    #sentence = "artist is blah blah and album is blah and title is blah .".split()
    #sentence = "artist = blah and not played this month || artist = blah .".split()
    #sentence = "play count greater than 5 and not played this week .".split()
    sentence = "create playlist play count greater than 5 .".split()
    #sentence = "not played this year or artist matches s t p .".split()
    #sentence = "hack the planet .".split()

    table, index = CKYRecognizer(cnf, sentence, "START")
    if index != None:
        tree = build_tree(sentence, table, index)
        print_tree(tree, 2, 80)
        rebalance_tree(tree)
        print_tree(tree, 2, 80)
    else:
        print("no match")

    #generate_sentences(5, cnf, read_grammar(grammar_path))

def generate_sentences(num_sentences, cnf, cfg):
    # normalize the cfg weights between 0 and 1
    cfgg = defaultdict(list)
    for key in cfg.keys():
        wprods = cfg[key]
        totalw = sum( w for w,p in wprods )
        cfgg[key] = [ (w/totalw,p) for w,p in wprods ]

    for i in range(num_sentences):
        sentence = generate(cfgg)
        print("sentence:", ' '.join(sentence))
        table, index = CKYRecognizer(cnf, sentence, "START")
        if index != None:
            print("matched")
            tree = build_tree2(sentence, table, index)
            #print_tree(tree, 2, 80)
        else:
            print("error")
    return;

if __name__ == '__main__':

    main()
