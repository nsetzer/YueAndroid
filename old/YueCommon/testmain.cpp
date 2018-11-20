
#include "yue/core/search/search_test.hpp"
#include "yue/core/util/dateutil.h"
#include "yue/core/util/yuetest.h"

namespace yue {
namespace core {

namespace util {
    int yuetest_datedelta();
}

}
}


int main()
{
/*
    yue::core::util::time_point_t t = yue::core::util::mktime(2017,7,7);
    std::cout << yue::core::util::formatTimePoint("%y/%m/%d",t) << std::endl;
    std::time_t tt = std::chrono::system_clock::to_time_t(t);
    std::cout << std::ctime(&tt) <<  std::endl;

    long y=2017,m=11,d=31;
    //yue::core::util::dateDelta(y,m,d,0,0,0);
    yue::core::util::parseRelativeDateDelta("-4w",y,m,d);
    std::cout << y << " " << m << " " << d << " " << std::endl;

    unsigned long dt =  yue::core::util::dateToEpochTime(2000,1,2);
    yue::core::util::dateFromEpochTime(dt,y,m,d);
    std::cout << y << " " << m << " " << d << " " << std::endl;
*/
    yue::core::util::yuetest_datedelta();

    yue::core::SearchGrammarTest test;
    test.run_test();

    return 0;
}
