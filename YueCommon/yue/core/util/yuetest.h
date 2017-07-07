

#ifndef YUE_CORE_UTIL_YUETEST_H
#define YUE_CORE_UTIL_YUETEST_H

#include <string>

#define YUE_TEST(X) int yuetest_ ##X ( void )

#define YUE_TEST_BEGIN() \
    int error = 0

#define YUE_TEST_RETURN() error

#define assert_true(x) do { \
    auto _x = x; \
    if (!_x) { \
        std::cout << "     " << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl; \
        std::cout << "[  ] " << #x << " := " << _x << std::endl; \
        error++; } else { \
    std::cout << "[OK] " << #x << std::endl; }} while(0)
#define assert_false(x) assert_true(!(x))

#define assert_equal(x,y) do { \
    auto _x = x; \
    auto _y = y; \
    if (_x==_y) { \
        std::cout << "[OK] " << #x << std::endl; \
    } else { \
        std::cout << "[  ] " << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl; \
        std::cout << "     " << #x << " != " << #y << std::endl; \
        std::cout << "     " << #x << " := " << _x << std::endl; \
        std::cout << "     " << #y << " := " << _y << std::endl; \
    } } while(0)

#define assert_notequal(x,y) do { \
    auto _x = x; \
    auto _y = y; \
    if (_x!=_y) { \
        std::cout << "[OK] " << #x << std::endl; \
    } else { \
        std::cout << "[  ] " << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << std::endl; \
        std::cout << "     " << #x << " == " << #y << std::endl; \
        std::cout << "     " << #x << " := " << _x << std::endl; \
        std::cout << "     " << #y << " := " << _y << std::endl; \
    } } while(0)

#define assert_date_equal(a,b,c,x,y,z) do {\
    if ((a)==(x) && (b)==(y) && (c)==(z) ) { std::cout << "[OK] "; \
    } else { std::cout << "[  ] "; } \
    std::cout << a << "==" << x << " && " \
              << b << "==" << y << " && " \
              << c << "==" << z << std::endl;} while(0)

#define assert_date_notequal(a,b,c,x,y,z) assert_false( \
    (a)==(x) && (b)==(y) && (c)==(z) )

#endif