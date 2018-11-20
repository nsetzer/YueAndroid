
checks='*'
checks="$checks,-modernize-raw-string-literal"
checks="$checks,-google-readability-todo"
checks="$checks,-google-readability-braces-around-statements"
checks="$checks,-readability-braces-around-statements"
checks="$checks,-misc-macro-parentheses"

clang-tidy --checks="$checks" \
    parser.cpp --  \
    -std=c++11 -I../../..
