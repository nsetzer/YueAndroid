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
    Q_PROPERTY(QString defaultQuery READ defaultQuery WRITE setDefaultQuery NOTIFY defaultQueryChanged)

protected:
    static int UniqueIdRole;
public:

    explicit LibraryTreeListModel(QObject *parent = nullptr);

    Q_INVOKABLE bool createPlaylist(bool shuffle = true);
    Q_INVOKABLE void search(QString query);

    QVariant data(const QModelIndex &index, int role/* = Qt::DisplayRole*/) const;
    QHash<int, QByteArray> roleNames() const;

    QString lastError() {return m_lastError;}


    QString defaultQuery() { return m_defaultQuery; }
    void setDefaultQuery(QString query) {
        m_defaultQuery = query;
        emit defaultQueryChanged();
    }

signals:
    void errorStatusChanged();
    void defaultQueryChanged();
    void searchResult(QList<yue::bell::LibraryTreeNode*> forest);

private:
    void searchImpl(QString query);
    QString m_lastError;
    void collectSelectedSongs(QMap<yue::bell::Database::uid_t, QString>& groups, yue::bell::LibraryTreeNode* node );
    void onDefaultQueryChanged();

    QString m_defaultQuery="";

};

} // qtcommon
} // yue

#endif // YUE_QTCOMMON_TREELISTMODELBASE_H
