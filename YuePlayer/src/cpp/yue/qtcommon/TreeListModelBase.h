#ifndef YUE_QTCOMMON_TREELISTMODELBASE_H
#define YUE_QTCOMMON_TREELISTMODELBASE_H

#include "yue/global.h"
#include "yue/bell/LibraryTreeNode.hpp"
#include <QObject>
#include <QAbstractListModel>

namespace yue {
namespace qtcommon {

class YUECOMMON_EXPORT TreeListModelBase : public QAbstractListModel
{
    Q_OBJECT



    Q_PROPERTY(bool anySelected READ anySelected() NOTIFY selectionChanged)
    Q_PROPERTY(bool allSelected READ allSelected() NOTIFY selectionChanged)
public:
    static int SongIdRole;
    static int CheckRole;
    static int ExpandedRole;
    static int DepthRole;
    static int ChildCountRole;
    static int RatingRole;
    static int UserRole;
public:

    explicit TreeListModelBase(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);


    QHash<int, QByteArray> roleNames() const;

    void setNewData(QList<yue::bell::LibraryTreeNode*> lst);

    Q_INVOKABLE void toggleExpandItem(int index);
    void _expand_item(yue::bell::LibraryTreeNode* node);
    Q_INVOKABLE void toggleCheckState(int index);
    Q_INVOKABLE void checkAll(bool bAll);
    bool anySelected();
    bool allSelected();

    Q_INVOKABLE void expandAll(bool bAll);
    bool anyExpanded();
    bool allExpanded();
signals:
    //void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int> ());
    void onExpand();
    void selectionChanged();

public slots:

protected:
    QList<yue::bell::LibraryTreeNode*> m_forest;
    QList<yue::bell::LibraryTreeNode*> m_tabledata;
};


} // qtcommon
} // yue

#endif // YUE_QTCOMMON_TREELISTMODELBASE_H
