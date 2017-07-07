
#ifndef YUE_CORE_UTIL_RANDOM_H
#define YUE_CORE_UTIL_RANDOM_H

#include <random>

namespace yue {
namespace core {

class Random {
    std::random_device rd;
    std::mt19937 m_gen;
public:
    Random()
        : rd()
        , m_gen(rd())
    {}

    void seed(const char* bytes) {
        unsigned int hash = 5381;
        int c;
        while((c = (*bytes++))) {
            hash = (hash*31) + c;
        }
        seed(hash);
    }

    void seed(unsigned int seed) {
        m_gen.seed(seed);
    }

    /**
     * @returns an integer x such that low <= x <= high;
     */
    int randInt(int low, int high) {
        std::uniform_int_distribution<int> dis(low,high);
        return dis(m_gen);
    }
    /**
     * @returns a float x such that low <= x <= high;
     */
    float rand(float low=0.0, float high=1.0) {
        std::uniform_real_distribution<float> dis(low,high);
        return dis(m_gen);
    }
};

} // namespace core
} // namespace yue
#endif
