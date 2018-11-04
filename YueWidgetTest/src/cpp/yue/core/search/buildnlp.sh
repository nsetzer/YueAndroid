

g++ -std=c++11 -g -I../../.. \
    grammar.cpp rules.cpp \
    nlpgrammar.cpp nlpgrammar_test.cpp \
    ../util/dateutil.cpp \
    -o testnlp