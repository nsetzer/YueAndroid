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

todo: EarlyParser should have a table of lists
store the index in the list that produces a new rule

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

State = namedtuple("State",['nonterm','dot','prod','start','weight','k'])

class ParseError(ValueError):
    pass

class EarlyParser(object):
    """ implementation of an early parser """
    def __init__ (self, grammar, root="START"):
        self.grammar = grammar
        self.root = root

    def parse (self, toks):
        # a state is:
        #   nonterm, dot, prod, start
        # nonterm: a nonterminal rule (e.g. Noun, S1)
        # dot: index into prod, when dot=len(prod) the rule is 'complete'
        # start: index where the rule first matched
        #
        # ("START", 0, (S1,), 0, None)
        # use a list to maintain order new states are added
        self.tokens = toks
        self.states = [ list() for i in range(len(toks)+1) ]
        for w,prod in self.grammar[self.root]:
            state = State(self.root,0,prod,0,w,-1)
            self.states[0].append(state)

        for i in range(len(toks)):
            for k,state in enumerate(self.states[i]):
                _, dot, prod, _, _, _ = state
                if dot >= len(prod):
                    self._complete(i,state)
                elif not self.is_term(prod[dot]):
                    self._predict(i,k,state)
                elif self.is_term(prod[dot]):
                    self._scan(i,state)

        if len(self.states[-1]) == 0:
            raise ParseError('unable to parse')

        # maybe this isnt an error
        if len(self.states[-1]) > 1:
            raise ParseError('too many options ... %d'%len(self.states[-1]))

        return True

    def _append_state(self,states,state):
        nt, dt, p, s, w, k = state
        for i in range(len(states)):
            st = states[i]
            if st.nonterm==nt and st.prod==p and st.dot==dt and st.start==s:
                if w < st.weight:
                    states[i] = state
                return
        states.append(state)

    def _complete(self,i,state):
        for st in self.states[state.start]:
            self._complete_state(state.nonterm,st,self.states[i])

    def _complete_state(self,name,state,states):
        nonterm, dot, prod, start, w, k = state
        if dot < len(prod) and prod[dot] == name:
            new_state = State(nonterm, dot+1, prod, start, w, k)
            self._append_state(states,new_state)

    def _predict(self,i,k,state):
        term = state.prod[state.dot]
        for w,new_prod in self.grammar[term]:
            #print("predict",term,0, new_prod, self.tokens[i])
            # look ahead to prevent adding states which cannot complete
            # TODO: extend to N look ahead, by check all terminals
            if new_prod[0] not in self.grammar and new_prod[0] != self.tokens[i]:
                continue

            new_state = State(term, 0, new_prod, i, state.weight+w, k)
            self._append_state(self.states[i],new_state)

    def _scan(self,i,state):
        nonterm, dot, prod, start, w, k =  state

        if self.tokens[i] != prod[dot]:
            return
        # push to next state
        new_state = State(nonterm, dot+1, prod, start, w, k)
        self._append_state(self.states[i+1],new_state)

    def is_term(self,tok):
        """ true if token is a terminal token
            only non-terminal rules are keys in the grammar
        """
        return tok not in self.grammar

    def _get_completed_states(self):

        for i,states in enumerate(self.states):
            print(i)
            for state in states:
                print(state)
        completed_states = []
        for i,states in enumerate(self.states):
            next = []
            for st in states:
                if st[1] == len(st[2]):
                    next.append(st)
            # need to process states in reverse order.
            completed_states.append(list(reversed(next)))
        return completed_states

    def build_tree (self):
        # remove non-completed states
        states = self._get_completed_states();
        idx = len(states)-1 # index of last token
        state = states[-1][0]
        print("num final states",len(states[-1]))
        tree,end = self._walk_states_old(states,state[0],idx)

        return tree

    def _walk_states(self, states, state):
        # TODO: somehow, k indicates the index in self.states[start]
        # that produced the given state, maybe that can be used to
        # do a better job of walking completed states?

        st_name, st_dot, st_prod, st_end, w, k = state


        i=0
        while i < len(states[end]):
            state = states[end][i]
            st_name, _, st_prod, _, w, k = state
            # find rule matching this nonterm
            if st_name == nonterm:
                states[end].pop(i) # consume this rule (prevent loops)
                root = []
                for term in reversed (st_prod):
                    if self.is_term(term):
                        # leaf on a tree is a terminal node
                        root.insert (0, term)
                        end -= 1
                    else:
                        # find a state where term completed
                        # create a subtree with term as the root
                        tree, end = self._walk_states(states, term, end)
                        root.insert (0, tree)
                return [nonterm,] + root, end
            i += 1

        raise ParseError("unexpected end of function")

    def _walk_states_old(self, states, nonterm, end):
        # pop states as they are used to prevent a loop
        # states are processed in reverse to guarantee the correct
        # tree is built. (states closer to the root of the tree
        # are finished last, while states closer to the leaves of the tree
        # complete first)
        i=0
        while i < len(states[end]):
            state = states[end][i]
            st_name, _, st_prod, _, w, k = state
            # find rule matching this nonterm
            if st_name == nonterm:
                states[end].pop(i) # consume this rule (prevent loops)
                root = []
                for term in reversed (st_prod):
                    if self.is_term(term):
                        # leaf on a tree is a terminal node
                        root.insert (0, term)
                        end -= 1
                    else:
                        # find a state where term completed
                        # create a subtree with term as the root
                        tree, end = self._walk_states_old(states, term, end)
                        root.insert (0, tree)
                return [nonterm,] + root, end
            i += 1

        raise ParseError("unexpected end of function")

def rebalance_tree(tree):

    i=0;
    while i < len(tree) :
        child = tree[i]
        if child[0].startswith("SEARCHTERM"):
            tree.pop(i)
            for j, item in enumerate(child[1:]):
                tree.insert(i+j,item)
        i+= 1


    for child in tree[1:]:
        if isinstance(child,list):
            rebalance_tree(child)

    i=1;
    while i < len(tree):

        child = tree[i]

        # combine value and text into a single node, TEXT
        if child[0] == "VALUE":
            tree[i] = child[1]
            i += 1

        elif tree[0] == "TEXT" and child[0] == "TEXT":
            tree.pop(i)
            for item in child[1:]:
                tree.insert(i,item)
                i+= 1

        # combine ternary operations into binary nodes with the
        # operation as the root
        elif child[0] == "OPERATION":
            rhs = tree.pop(i+1)
            lhs = tree.pop(i-1)
            child.append(lhs)
            child.append(rhs)

        # Hoist expressions that would join two logical operations
        elif child[0] == "_join":
            child.pop(0)
            rhs = tree.pop(i+1)
            lhs = tree.pop(i-1)
            child.append(lhs)
            child.append(rhs)
        else:
            i += 1



        # ---

    for term in ("and","or"):
        i=len(tree)-1;
        while i >=1 :
            child = tree[i]
            if tree[0].startswith("SEARCHTERM") and \
                 (child[0] == term):
                tree[0]=child[0]
                tree.pop(i)
                for j, item in enumerate(child[1:]):
                    tree.insert(i+j,item)
            i-= 1



def main():

    grammar_path = "grammar.txt"
    grammar = read_grammar(grammar_path)

    parser = EarlyParser(grammar,"START")

    #sentence = "artist is OOV and album is OOV and not played today .".split()
    sentence = "artist greater than OOV  .".split()
    #sentence = "OOV OOV OOV OOV .".split()


    parser.parse(sentence);

    tree = parser.build_tree()
    print_tree(tree, 2, 80);
    rebalance_tree(tree)
    print_tree(tree, 2, 80);

if __name__ == '__main__':

    main()
