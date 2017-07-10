

#ifndef YUE_CORE_UTIL_DATEUTIL_H
#define YUE_CORE_UTIL_DATEUTIL_H

#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>
#include <iostream>

namespace yue {
namespace core {
namespace util {

class ydate_t {
public:
    ydate_t()
        : year(1970), month(1), day(1)
    {}
    ydate_t(long y, long m, long d)
        : year(y), month(m), day(d)
    {}
    ~ydate_t() = default;

    long year;
    long month;
    long day;

    bool operator==(const ydate_t& other) const {
        return other.year==year && \
               other.month==month && \
               other.day==day;

    }

    friend std::ostream& operator<<(std::ostream& os, const ydate_t& date) {
        os << date.year << '/' << date.month << '/' << date.day;
        return os;
    }
};

class ytime_t {
public:
    int h;
    int m;
    int s;
};

typedef std::chrono::time_point<std::chrono::system_clock> time_point_t;
typedef  std::chrono::duration<double> duration_t;

void msleep(long ms);

time_point_t currentTimePoint();

class Timer
{
    time_point_t m_start;
public:
    Timer()
        : m_start(currentTimePoint())
    {}
    virtual ~Timer() = default;

    void reset() {
        m_start = currentTimePoint();
    }

    double elapsed() {
        duration_t d = currentTimePoint() - m_start;
        return d.count();
    }

};

bool isLeapYear(int year);
int daysInMonth(int year, int month);

long dateDayNumber(const ydate_t& date);
ydate_t dateFromDayNumber(long g);
bool dateIsValid(const ydate_t& date);

ydate_t dateDelta(const ydate_t& date, long dy, long dm, long dd);
ydate_t dateDelta(const ydate_t& date, long dd);

unsigned long dateToEpochTime(const ydate_t& date);
ydate_t dateFromEpochTime(unsigned long dt);

time_point_t mktime(const ydate_t& date);

ydate_t extractDate(const time_point_t datetime);

void parseRelativeDateDelta(std::string sDateDelta, ydate_t& date, bool negate=false);

} // namespace util
} // namespace core
} // namespace yue


#endif