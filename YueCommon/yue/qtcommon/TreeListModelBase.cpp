
#include <QDir>
#include <QDebug>
#include "yue/qtcommon/TreeListModelBase.hpp"
#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
namespace yue {
namespace qtcommon {

int TreeListModelBase::CheckRole      = Qt::UserRole+1;
int TreeListModelBase::ExpandedRole   = Qt::UserRole+2;
int TreeListModelBase::DepthRole      = Qt::UserRole+3;
int TreeListModelBase::ChildCountRole = Qt::UserRole+4;
int TreeListModelBase::UserRole       = TreeListModelBase::ChildCountRole;
/*
 * experiment:
 *  BasicListModel<T>
 *  StringListModel : public BasicListModel<QString>
 *  setNewData(QList<T>& lst)
 *
 *  make a class which has a property of type abstract model
 *  the class interacts with the model and can be placed in
 *  the qml. the list view and class share a model
 */
TreeListModelBase::TreeListModelBase(QObject *parent)
    : QAbstractListModel(parent)
{




    /*for (int i=0; i<10; i++)
    {
        LibraryTreeNode* parent = new LibraryTreeNode("parent " + QString::number(i),0);
        for (int j=0; j<3; j++) {
            LibraryTreeNode* child = new LibraryTreeNode("child" + QString::number(j),1);
            parent->addChild(child);
            for (int k=0; k<3; k++) {
                LibraryTreeNode* gchild = new LibraryTreeNode("gchild" + QString::number(k),2);
                child->addChild(gchild);
            }
        }
        m_forest.push_back(parent);
    }
    */
    //QList<yue::bell::LibraryTreeNode*> forest = yue::bell::Library::instance()->queryToForest("");
    //setNewData(forest);


}

int TreeListModelBase::rowCount(const QModelIndex &parent/* = QModelIndex()*/) const
{
    Q_UNUSED(parent);
    return m_tabledata.size();
}

int TreeListModelBase::columnCount(const QModelIndex &parent/* = QModelIndex()*/) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant TreeListModelBase::data(const QModelIndex &index, int role/* = Qt::DisplayRole*/) const
{
    if (index.row() < 0 || index.row() >= rowCount())
        return QVariant();

    if (role == TreeListModelBase::CheckRole)
        return (int)m_tabledata[index.row()]->getCheckState();

    if (role == TreeListModelBase::ExpandedRole)
        return m_tabledata[index.row()]->isExpanded();

    if (role == TreeListModelBase::DepthRole)
        return m_tabledata[index.row()]->getDepth();

    if (role == TreeListModelBase::ChildCountRole) {
        QVariant v;
        v.setValue(m_tabledata[index.row()]->numChildren());
        return v;
    }

    if (role == Qt::DisplayRole)
        return m_tabledata[index.row()]->text();

    return QVariant();
}

bool TreeListModelBase::setData(const QModelIndex &index, const QVariant &value, int role/* = Qt::EditRole*/)
{
    Q_UNUSED(value);
    if (role != Qt::EditRole)
        return false;

    if (index.row() < 0 || index.row() >= rowCount())
        return false;

    //m_tabledata[index.row()] = value.toString();
    //emit dataChanged(index,index, {role,});
    return false;
}

QHash<int, QByteArray> TreeListModelBase::roleNames() const
{

    QHash<int, QByteArray> roles(QAbstractItemModel::roleNames());

    roles[TreeListModelBase::CheckRole] = "checkState";
    roles[TreeListModelBase::ExpandedRole] = "isExpanded";
    roles[TreeListModelBase::DepthRole] = "depth";
    roles[TreeListModelBase::ChildCountRole] = "childCount";
    return roles;
}

void TreeListModelBase::setNewData(QList<yue::bell::LibraryTreeNode*> lst)
{
    emit beginResetModel();
    // free any memory allocated from the previous search
    if (m_forest.size()>0) {
        m_tabledata.clear();
        while (m_forest.size()>0) {
            yue::bell::LibraryTreeNode* node = m_forest.back();
            delete node;
            m_forest.pop_back();
        }
    }

    m_forest = lst;
    //TODO memory leaks
    // use custom node list that deletes pointers ?
    m_tabledata = QList<yue::bell::LibraryTreeNode*>(lst);
    //m_tabledata.push_back(new yue::bell::LibraryTreeNode(-1,"",0,NULL));
    //m_tabledata.push_back(new yue::bell::LibraryTreeNode(-1,"",0,NULL));
    //m_tabledata.push_back(new yue::bell::LibraryTreeNode(-1,"",0,NULL));
    emit endResetModel();
}

void TreeListModelBase::toggleExpandItem(int idx)
{
    yue::bell::LibraryTreeNode* node = m_tabledata[idx];

    emit onExpand();

    if (node->isExpanded())
    {
        // remove all child nodes (including grandchildren)

        int remove_idx = idx + 1;
        // if index out of bounds or the next element is not a child
        // of the current item, return early
        if (remove_idx >= m_tabledata.size() ||
            !m_tabledata[remove_idx]->isDescendant(node))
        {
            return;
        }

        node->setExpanded(false);
        emit dataChanged(index(idx), index(idx), {TreeListModelBase::ExpandedRole});

        // find the last child (or grand child) index
        int remove_end = remove_idx;
        while (remove_end+1 < m_tabledata.size() &&
               m_tabledata[remove_end+1]->isDescendant(node))
        {
            remove_end++;
        }

        beginRemoveRows(QModelIndex(), remove_idx, remove_end);
        m_tabledata.erase(m_tabledata.begin()+remove_idx,
                          m_tabledata.begin()+remove_end+1);
        endRemoveRows();

    }
    else
    {
        // insert all children (and grandchildren) that are visible.

        node->setExpanded(true);
        emit dataChanged(index(idx), index(idx), {TreeListModelBase::ExpandedRole});

        QList<yue::bell::LibraryTreeNode*> children;
        node->getVisibleChildren(children);

        if (children.size()==0)
            return;

        int insert_idx = idx+1;
        int insert_end = insert_idx + children.size() - 1;


        beginInsertRows(QModelIndex(), insert_idx, insert_end);
        for (int i=0; i < static_cast<int>(children.size()); i++)
        {
            m_tabledata.insert(insert_idx + i, children[i]);
        }
        endInsertRows();

    }
}

void TreeListModelBase::toggleCheckState(int idx)
{
    yue::bell::LibraryTreeNode* nd = m_tabledata[idx];
    nd->setCheckState((nd->getCheckState()==Qt::Unchecked)?Qt::Checked:Qt::Unchecked);
    emit dataChanged(index(0), index(m_tabledata.size()-1), {TreeListModelBase::CheckRole});
    emit selectionChanged();
}

void TreeListModelBase::checkAll(bool state)
{
    for (yue::bell::LibraryTreeNode* root : m_forest) {
        root->setCheckState(state?Qt::Checked:Qt::Unchecked);
    }
    emit dataChanged(index(0), index(m_tabledata.size()-1), {TreeListModelBase::CheckRole});
    emit selectionChanged();
}

bool TreeListModelBase::anySelected()
{
    for (yue::bell::LibraryTreeNode* root : m_forest) {
        if (root->getCheckState() == Qt::Checked ||
            root->getCheckState() == Qt::PartiallyChecked)
            return true;
    }
    return false;
}

bool TreeListModelBase::allSelected()
{
    for (yue::bell::LibraryTreeNode* root : m_forest) {
        if (root->getCheckState() != Qt::Checked)
            return false;
    }
    return true;
}


} // qtcommon
} // yue
