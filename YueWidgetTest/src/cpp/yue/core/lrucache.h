

#ifndef YUE_BELL_LRUCACHE
#define YUE_BELL_LRUCACHE

#include <deque>
#include <map>

namespace yue {
namespace core {

/**
 * TODO: in testing this occasionally returns corrupt data
 * as a result of update() but sometimes even when update is not called.
 * This was intended for integer and pointer types only
 * Usually a QQueue and removeOne/enqueue is good enough for a lru cache.
 */
template<typename T>
class lrucache
{


public:
    lrucache()
        : m_lst()
        , m_map()
    {};
    ~lrucache()=default;

    size_t size() const {
        return m_lst.size();
    }
    void insert(T& item) {
        m_map[item] = m_lst.size();
        m_lst.push_back(item);
    }
    void update(T& item) {
        if (m_map.count(item)==0) {
            throw std::runtime_error("key not found");
        }
        size_t index = m_map[item];
        m_lst.erase(m_lst.begin()+index);
        m_map[item] = m_lst.size();
        m_lst.push_back(item);
    }
    T& pop() {
        T& item = m_lst.front();
        m_lst.pop_front();
        m_map.erase(item);
        return item;
    }
    void remove(const T& item) {
        size_t index = m_map[item];
        m_map.erase(item);
        m_lst.erase(m_lst.begin()+index);
    }


private:
    // maintain a list of elements.
    // higher indices indicate recent use.
    std::deque<T> m_lst;
    // map an element to the index in the list
    std::map<T,size_t> m_map;
};

} // namespace core
} // namespace yue

#endif
