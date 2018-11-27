
#ifndef YUE_BELL_PLAYLIST_HPP
#define YUE_BELL_PLAYLIST_HPP

#include <QObject>
#include <QString>
#include <QList>
#include <QPair>
#include <QSharedPointer>
#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
#include "yue/bell/LibraryTreeNode.hpp"
#include "yue/global.h"

namespace yue {
namespace bell {

class YUECOMMON_EXPORT Playlist;

class YUECOMMON_EXPORT PlaylistManager : public QObject
{
    Q_OBJECT

    static const QString current_playlist_name;
    static PlaylistManager* m_instance;

    Library* m_library;


public:
    PlaylistManager(Library* library);
    ~PlaylistManager();

    static PlaylistManager* create( Library* library = nullptr ) {
        if (library == NULL)
            library = Library::instance();
        m_instance = new PlaylistManager( library );
        return m_instance;
    }
    static PlaylistManager* instance( void ) {
        return m_instance;
    }

    QSharedPointer<Playlist> openCurrent();
    QSharedPointer<Playlist> open(QString name);

};


class YUECOMMON_EXPORT Playlist : public QObject
{
    Q_OBJECT

    Database* m_db;
    Database::plid_t m_plid;
    Library* m_library = nullptr;
    QString m_name;
private:
    Playlist(Library* library, Database::plid_t plid, QString name);
public:

    ~Playlist();

    QString name() { return m_name; }

    void set(QList<Database::uid_t> lst, size_t current_index=0);
    void clear();
    size_t size();

    Database::uid_t get(int idx);
    Database::uid_t setCurrent(int idx);
    QPair<Database::uid_t,size_t> current();
    QPair<Database::uid_t,size_t> next();
    QPair<Database::uid_t,size_t> prev();
    void insert(int idx, Database::uid_t uid);
    void remove(int idx);
    void move(int src, int tgt);

    QSqlQuery select();

    QList<Database::uid_t> toList() const;



private:
    bool _insert_uid(int idx, Database::uid_t uid);
    bool _remove_one(int idx);
    bool _move_one(int src, int tgt);

    friend class PlaylistManager;
};



} // namespace bell
} // namespace yue

#endif
