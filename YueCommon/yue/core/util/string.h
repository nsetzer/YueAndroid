#ifndef YUE_CORE_UTIL_STRING_H
#define YUE_CORE_UTIL_STRING_H

#include <string>
#include <algorithm>
namespace yue {
namespace core {
namespace util {


// trim from start (in place)
static inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s)
{
    ltrim(s);
    rtrim(s);
}

static inline bool startswith(const std::string& str, const std::string& substr)
{
    return str.find(substr) == 0;
}

static inline bool startswith(const std::string& str, const char* substr)
{
    return str.find(substr) == 0;
}

template<typename T>
void split(const std::string &s, char delim, T result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}


} // util
} // namespace core
} // namespace yue

#endif // YUE_CORE_UTIL_STRING_H