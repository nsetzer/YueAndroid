#ifndef YUE_QTCOMMON_LIBRARYTREELISTLMODEL_H
#define YUE_QTCOMMON_LIBRARYTREELISTLMODEL_H

#include "yue/core/shuffle.hpp"
#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
#include "yue/bell/playlist.hpp"
#include "yue/qtcommon/TreeListModelBase.hpp"

namespace yue {
namespace qtcommon {

class YUECOMMON_EXPORT LibraryTreeListModel : public TreeListModelBase
{
    Q_OBJECT
    Q_PROPERTY(QString lastError READ lastError NOTIFY errorStatusChanged)

public:

    explicit LibraryTreeListModel(QObject *parent = nullptr);

    Q_INVOKABLE bool createPlaylist();
    Q_INVOKABLE void search(QString query);

    QString lastError() {return m_lastError;}

signals:
    void errorStatusChanged();

private:
    QString m_lastError;
    void collectSelectedSongs(QMap<yue::bell::Database::uid_t, QString>& groups, yue::bell::LibraryTreeNode* node );

};

} // qtcommon
} // yue

#endif // YUE_QTCOMMON_TREELISTMODELBASE_H
