
#include <stdlib.h>
#include <time.h>
#include "yue/core/util/dateutil.h"

namespace yue {
namespace core {
namespace util {


void msleep(long ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

time_point_t currentTimePoint()
{
    return  std::chrono::system_clock::now();
}

/**
 * http://en.cppreference.com/w/cpp/chrono/c/strftime
 * %Y -- 4 digit year
 * %y -- last 2 digits of the year
 * %m -- month of year (01..12)
 * %B -- month as locale string (e.g. October)
 * %b -- abbreviated month as locale string (e.g. Oct)
 * %d -- day of month
 * %H -- hour (00..23)
 * %I -- hour (01..12)
 * %M -- minutes (00..59)
 * %S -- seconds (00..60)
 */
std::string formatTimePoint(std::string format, time_point_t datetime)
{
    char buf[128];
    time_t tt = std::chrono::system_clock::to_time_t(datetime);
    tm local_tm = *localtime(&tt);

    size_t length = strftime(buf,128, format.c_str(), &local_tm);
    if (length > 0)
        return std::string(buf);
    return "";
}

/**
 * @param year the natural year number (e.g. 2000)
 * @return true if the given year is a leap year.
 */
bool isLeapYear(int year)
{
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

/**
 * @param year the natural year number (e.g. 2000)
 * @param month the natural number of the month (e.g. 1=january)
 * @return the number of days in the given month for a given year
 */
int daysInMonth(int year, int month)
{
    switch (month) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return 31;
        case 2:
            return isLeapYear(year)?29:28;
        default:
            return 30;
    }
}

//https://alcor.concordia.ca//~gpkatch/gdate-algorithm.html
long dateDayNumber(long y, long m, long d)
{
    m = (m + 9) % 12;
    y = y - m/10;
    return 365*y + y/4 - y/100 + y/400 + (m*306 + 5)/10 + ( d - 1 );
}

void dateFromDayNumber(long g, long& y, long& m, long& d)
{
    long dd, mm;
    y = (10000*g + 14780)/3652425;
    dd = g - (365*y + y/4 - y/100 + y/400);
    if (dd < 0) {
        y = y - 1;
        dd = g - (365*y + y/4 - y/100 + y/400);
    }
    mm = (100*dd + 52)/3060;
    m = (mm + 2)%12 + 1;
    y = y + (mm + 2)/12;
    d = dd - (mm*306 + 5)/10 + 1;
}

/**
 * @returns true if the given date is a valid calendar date
 *
 * @remark for example, (2000,11,31) is an invalid date
 *         and (2000,12,1) is a valid date
 */
bool dateIsValid(long y, long m, long d)
{
    long yy,mm,dd;
    dateFromDayNumber(dateDayNumber(y,m,d),yy,mm,dd);
    return y==yy && m==mm && d==dd;
}

/**
 * @param y input/output year (e.g. 2000)
 * @param m input/output month (1..12)
 * @param d input/output day (1..31)
 * @param dy input year delta. add or subtract this many years
 * @param dm input month delta. add or subtract this many months
 * @param dd input day delta. add or subtract this many days

 * @returns void. sets y,m,d to a new date tripple by adding or subtracting
 * the given number of years, months, and/or days.
 *
 * @remark this will correct an invalid date for example
 * an input of (2000,11,31,0,0,0) will return (2000,12,1)
 */
ydate_t dateDelta(const ydate_t& date, long dy, long dm, long dd)
{
    long y = date.year;
    long m = date.month;
    long d = date.day;

    y += dy;
    if (dm != 0) {
        // add/sub 1 to convert range 1..12 to 0..11 for math reasons
        int t = (m - 1 + dm);
        y = y + t/12;
        m = t%12 + 1;
    }

    // modulo fix the day by rolling up, feb 29 to march 1
    // or july 32 to aug 1st, if needed
    int days = daysInMonth(y,m);
    if (d>days) {
        d -= days;
        m += 1;
    }

    // fix the year by rounding months that are out of bounds
    if (m > 12) {
        m -= 12;
        y += 1;
    }
    if (m <= 0) {
        m += 12;
        y -= 1;
    }

    long g = dateDayNumber(y,m,d);
    dateFromDayNumber(g+dd,y,m,d);

    return ydate_t(y,m,d);
}

ydate_t dateDelta(const ydate_t& date, long dd)
{
    long y,m,d;
    long g = dateDayNumber(date.year,date.month,date.day);
    dateFromDayNumber(g+dd,y,m,d);

    return ydate_t(y,m,d);
}

unsigned long dateToEpochTime(const ydate_t& date)
{
    time_t rawtime;
    struct tm * timeinfo;
    // initialize the struct to the local time information
    // then replace with the given date information
    time ( &rawtime );
    timeinfo = gmtime ( &rawtime );
    timeinfo->tm_year = date.year - 1900;
    timeinfo->tm_mon = date.month - 1;
    timeinfo->tm_mday = date.day;
    timeinfo->tm_hour = 0;
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 0;

    rawtime = mktime ( timeinfo );

    return static_cast<unsigned long>(rawtime);
}

ydate_t dateFromEpochTime(unsigned long dt)
{
    time_t rawtime = static_cast<time_t>(dt);
    tm utc_tm = *gmtime ( &rawtime );

    long y = utc_tm.tm_year + 1900;
    long m = utc_tm.tm_mon + 1;
    long d = utc_tm.tm_mday;
    return ydate_t(y,m,d);
}



// ----------------------------------------------------------------------------

time_point_t mktime(const ydate_t& date)
{

    time_t rawtime;
    struct tm * timeinfo;
    // initialize the struct to the local time information
    // then replace with the given date information
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    timeinfo->tm_year = date.year - 1900;
    timeinfo->tm_mon = date.month - 1;
    timeinfo->tm_mday = date.day;
    timeinfo->tm_hour = 0;
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 0;

    rawtime = mktime ( timeinfo );

    return std::chrono::system_clock::from_time_t( rawtime );
}

ydate_t extractDate(const time_point_t datetime)
{
    time_t tt = std::chrono::system_clock::to_time_t(datetime);
    //tm utc_tm = *gmtime(&tt);
    tm local_tm = *localtime(&tt);


    long y = local_tm.tm_year + 1900;
    long m = local_tm.tm_mon + 1;
    long d = local_tm.tm_mday;
    return ydate_t(y,m,d);
}

// ----------------------------------------------------------------------------

/**
 * e.g. ("4w") returns a time_point 4 weeks in the past
 * e.g. ("-4w") returns a time_point 4 weeks in the future
 * @returns a time_point representing a date
 */
void parseRelativeDateDelta(std::string sDateDelta, ydate_t& date, bool negate/*=false*/)
{
    //bool negate=false;
    long dy=0,dm=0,dd=0;
    std::string number;
    for (size_t i=0; i < sDateDelta.size(); i++) {
        char c = sDateDelta[i];
        if (c == '-') {
            negate = !negate;
        } else if (c == 'y') {
            dy += atoi(number.c_str());
            number = "";
        } else if (c == 'm') {
            dm += atoi(number.c_str());
            number = "";
        } else if (c == 'w') {
            dd += 7*atoi(number.c_str());
            number = "";
        } else if (c == 'd') {
            dd += atoi(number.c_str());
            number = "";
        } else if (c >= '0' && c <= '9') {
            number += c;
        }
    }

    if (number.size()>0)
        dd += atoi(number.c_str());

    if (negate) {
        dy*=-1;
        dm*=-1;
        dd*=-1;
    }

    date = dateDelta(date,dy,dm,dd);
}



} // namespace util
} // namespace core
} // namespace yue
