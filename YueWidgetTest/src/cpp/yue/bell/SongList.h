#ifndef YUE_BELL_SONGLIST_H
#define YUE_BELL_SONGLIST_H

#include <QObject>
#include <QList>
#include "yue/global.h"
#include "yue/bell/database.hpp"

namespace yue {
namespace bell {

template<typename T>
class YUECOMMON_EXPORT SongList
{
public:
    SongList(int index=0)
        : m_songs()
        , m_current_index(index)
    {}
    SongList(const QList<T>& lst, int index=0)
        : m_songs()
        , m_current_index(index)
    {
        m_songs = lst;
    }

    void move(int src, int tgt) {
        m_songs.move(src,tgt);
        if (src == m_current_index) {
            m_current_index = tgt;
        } else if (src < m_current_index && tgt >= m_current_index) {
            m_current_index--;
        } else if (src > m_current_index && tgt <= m_current_index) {
            m_current_index++;
        }
    }

    void remove(int idx) {
        m_songs.removeAt(idx);

        if (idx < m_current_index) {
            m_current_index--;
        } else if (m_current_index > m_songs.size()-1) {
            m_current_index = m_songs.size()-1;
        }
    }

    void clear() {
        m_songs.clear();
        m_current_index = 0;
    }

    void insert(int idx, T elem) {
        m_songs.insert(idx, elem);
        if (idx <= m_current_index) {
            m_current_index++;
        }
    }

    void push_back(T elem) {
        m_songs.push_back(elem);
    }

    int size() const { return m_songs.size(); }

    const QList<T>& elems() const { return m_songs; }

    T& operator[](int idx) {
        return m_songs[idx];
    }
    const T& operator[](int idx) const {
        return m_songs[idx];
    }

    int currentIndex() const { return m_current_index; }
    void setCurrentIndex(int idx ) { m_current_index = idx; }

private:
    QList<T> m_songs;
    int m_current_index;
};

} // bell
} // yue

#endif // YUE_BELL_SONGLIST_H
