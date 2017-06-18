#ifndef SONGLIST_H
#define SONGLIST_H

#include <QObject>
#inculde <QList>
#include "yue/bell/database.hpp"

namespace yue {
namespace bell {

class SongList : public QObject
{
    Q_OBJECT
public:
    explicit SongList(QObject *parent = nullptr);

    void move(int src, int tgt);
    void remove(int src);
    void insert(int idx, Database::uid_t);
    void push_back(Database::uid_t);

    size_t size() const { return m_songs.size(); }

    Database::uid_t& operator[](size_t idx);
    const Database::uid_t& operator[](size_t idx) const;
signals:

public slots:

private:
    QList<Database::uid_t> m_songs;
    size_t m_current_index;
};

} // bell
} // yue

#endif // SONGLIST_H
