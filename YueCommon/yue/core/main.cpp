
#include <ctime>
#include <iostream>
#include "yue/core/search/search_test.hpp"
#include "yue/core/shuffle.hpp"

struct elem_t {
    int x;
};
struct elemxy_t {
    int x;
    int y;
};

int
main(void) {

    //yue::core::SearchGrammarTest test;
    //test.run_test();
    //std::vector<struct elem_t> data = {{1},{2},{3},{1},{1},{2},{3},{1},{2},{3}};
    std::vector<long> factors = {10,20,30,40,50,100,1000,5000,10000};

    for (auto k : factors) {
        std::vector<struct elem_t> data;
        for (int j=0; j<k; j++) {
            for (int i=0; i < 10; i++) {
                data.push_back({i});
            }
        }

        //yue::core::shuffle<struct elem_t, int, ElementKey>( data, [](struct elem_t*){ return 0; } );
        {
        std::clock_t start = std::clock();
        std::vector<struct elem_t> temp = yue::core::shuffle<struct elem_t, int>( data , [](const struct elem_t& e){ return e.x;} );
        std::clock_t end = std::clock();
        std::cout << "1. " << data.size() << ", " << (end - start)/((float)CLOCKS_PER_SEC) << std::endl;
        }

        {
        std::clock_t start = std::clock();
        std::vector<struct elem_t> temp = yue::core::shuffle<struct elem_t, int>( data , [](const struct elem_t& e){ return e.x;} );
        std::clock_t end = std::clock();
        std::cout << "2. " << data.size() << ", " << (end - start)/((float)CLOCKS_PER_SEC) << std::endl;
        }
    }

    /*
    std::vector<struct elemxy_t> data = {{1,1},{1,2},{1,3},{2,1},{2,2},{2,3}};
    auto temp = yue::core::shuffle2<struct elemxy_t,int>(data,[](const struct elemxy_t& e){ return e.x;});
    for (auto& e : temp) {
        std::cout << e.x  << "," << e.y << "; ";
    }
    std::cout << std::endl;
    */

    return 0;
}