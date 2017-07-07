
sources="testmain.cpp"
sources="${sources} yue/core/search/*.cpp"
sources="${sources} yue/core/util/*.cpp"
sources="${sources} yue/core/*.cpp"

echo $sources
g++ -g -std=c++11 -I. $sources -o test