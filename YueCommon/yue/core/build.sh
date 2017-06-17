

g++ -Wall -Wfatal-errors -std=c++11 -ggdb \
    -I../.. \
    main.cpp \
    search/rules.cpp search/grammar.cpp song.cpp shuffle.cpp\
    search/search_test.cpp -o main
#g++ -Wall -Wfatal-errors -std=c++14 -ggdb -I../../.. -I../../alien/utf8 main.cpp rule.cpp parser.cpp song.cpp -o main
