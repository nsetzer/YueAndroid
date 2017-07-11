

#include <iostream>
#include "yue/core/util/dateutil.h"
#include "yue/core/util/yuetest.h"

namespace yue {
namespace core {
namespace util {

YUE_TEST(datedelta)
{
    YUE_TEST_BEGIN();

    long y,m,d;
    assert_true(dateIsValid(ydate_t(2000,1,1)));
    assert_false(dateIsValid(ydate_t(2000,11,31)));

    ydate_t dd;
    dd = ydate_t(2000,1,1);
    dd = dateDelta(dd, 0, 0, 1);
    assert_equal(dd,ydate_t(2000,1,2));

    dd = ydate_t(2000,1,1);
    dd = dateDelta(dd, 0, 0, -1);
    assert_equal(dd,ydate_t(1999,12,31));

    // test increment/decrement by month
    dd = ydate_t(2000,1,1);
    dd = dateDelta(dd, 0, 1, 0);
    assert_equal(dd,ydate_t(2000, 2,1));

    dd = ydate_t(2000,1,1);
    dd = dateDelta(dd, 0, -1, 0);
    assert_equal(dd,ydate_t(1999,12,1));

    dd = ydate_t(2000,1,1);
    dd = dateDelta(dd, 0, 30, 0);
    assert_equal(dd,ydate_t(2002, 7,1));

    dd = ydate_t(2000,1,1);
    dd = dateDelta(dd, 0, -30, 0);
    assert_equal(dd,ydate_t(1997, 7,1));

    // test increment/decrement by month
    dd = ydate_t(2000,1,1);
    dd = dateDelta(dd, -1, 0, 0);
    assert_equal(dd,ydate_t(1999,1,1));

    dd = ydate_t(2000,1,1);
    dd = dateDelta(dd, 1, 0, 0);
    assert_equal(dd,ydate_t(2001,1,1));

    {
        ydate_t date(2005,5,4);
        unsigned long epochtime = dateToEpochTime(date);
        assert_equal(epochtime,1115182800);
        ydate_t date2 = dateFromEpochTime(epochtime);
        assert_equal(date,date2);
    }

    { // add/subtract by year
        ydate_t date(2000,1,1);
        parseRelativeDateDelta("1y",date);
        assert_true(date==ydate_t(2001,1,1));
        parseRelativeDateDelta("-1y",date);
        assert_true(date==ydate_t(2000,1,1));
    }

    { // negate the direction for adding
        ydate_t date(2000,1,1);
        parseRelativeDateDelta("-1y",date,true);
        assert_true(date==ydate_t(2001,1,1));
        parseRelativeDateDelta("1y",date,true);
        assert_true(date==ydate_t(2000,1,1));
    }

    { // add/subtract by month
        ydate_t date(2000,1,1);
        parseRelativeDateDelta("1m",date);
        assert_true(date==ydate_t(2000,2,1));
        parseRelativeDateDelta("-1m",date);
        assert_true(date==ydate_t(2000,1,1));
    }

    { // add/subtract by week
        ydate_t date(2000,1,1);
        parseRelativeDateDelta("1w",date);
        assert_true(date==ydate_t(2000,1,8));
        parseRelativeDateDelta("-1w",date);
        assert_true(date==ydate_t(2000,1,1));
    }

    { // add/subtract by day
        ydate_t date(2000,1,1);
        parseRelativeDateDelta("1d",date);
        assert_true(date==ydate_t(2000,1,2));
        parseRelativeDateDelta("-1d",date);
        assert_true(date==ydate_t(2000,1,1));
    }

    YUE_TEST_END();
}



} // namespace util
} // namespace core
} // namespace yue

