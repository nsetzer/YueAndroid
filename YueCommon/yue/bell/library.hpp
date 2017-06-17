
#ifndef YUE_BELL_LIBRARY_HPP
#define YUE_BELL_LIBRARY_HPP

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QVariant>
#include "yue/core/song.hpp"
#include "yue/core/shuffle.hpp"
#include "yue/core/search/grammar.hpp"
#include "yue/bell/database.hpp"
#include "yue/bell/LibraryTreeNode.hpp"
#include "yue/bell/LibrarySearchGrammar.hpp"
#include "yue/global.h"


namespace yue {
namespace bell {

class YUECOMMON_EXPORT Library : public QObject
{
    Q_OBJECT

public:
    static Library* m_instance;

    Database* m_db = nullptr;
    LibrarySearchGrammar m_grammar;
public:

    Library(Database *db=nullptr);
    ~Library();

    static Library* create( Database* db=nullptr ) {
        if (db == NULL)
            db = Database::instance();
        m_instance = new Library( db );
        return m_instance;
    }
    static Library* instance( void ) {
        return m_instance;
    }

    size_t size();

    Database::uid_t insert(QMap<QString,QVariant> data);
    void update(Database::uid_t uid, QMap<QString,QVariant> data);

    void incrementPlaycount(Database::uid_t uid);

    QList<LibraryTreeNode*> queryToForest(QString querystr);

    QList<Database::uid_t> createPlaylist(QString query, size_t size = 0);

    Database* db() { return m_db; }

    static QList<Database::uid_t> shuffle(const QList<Database::uid_t>& songs,
                                          const QMap<Database::uid_t,QString>& groups) {
        return yue::core::shuffle<QList<Database::uid_t>, Database::uid_t,QString>(songs,
            [=](const Database::uid_t& uid){return groups[uid];});

    }

private:
    bool _insert(QMap<QString,QVariant> data, Database::uid_t& uid);
    bool _update(Database::uid_t uid, QMap<QString,QVariant> data);


    Database::uid_t _get_or_create_artist_id(QString name, QString sortkey);
    Database::uid_t _get_or_create_album_id(Database::uid_t artist, QString name);
};

} // namespace bell
} // namespace yue

#endif
