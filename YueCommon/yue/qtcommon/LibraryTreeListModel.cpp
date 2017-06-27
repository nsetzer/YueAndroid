
#include <QDir>
#include <QDebug>
#include "yue/qtcommon/LibraryTreeListModel.hpp"
#include "yue/bell/MediaCtrlBase.h"

namespace yue {
namespace qtcommon {

int LibraryTreeListModel::UniqueIdRole = LibraryTreeListModel::UserRole + 1;

LibraryTreeListModel::LibraryTreeListModel(QObject *parent)
    : TreeListModelBase(parent)
{

    //search("");
    connect(this,&LibraryTreeListModel::defaultQueryChanged,
            this,&LibraryTreeListModel::onDefaultQueryChanged);
}

/**
 * @brief LibraryTreeListModel::createPlaylist
 * @return true when a playlist is created
 */
bool LibraryTreeListModel::createPlaylist()
{
    LOG_FUNCTION_TIME();

    QMap<yue::bell::Database::uid_t, QString> groups;

    for (yue::bell::LibraryTreeNode* node : m_forest)
    {
        collectSelectedSongs(groups,node);
    }
    if (groups.size()==0) {
        return false;
    }

    auto lst = yue::bell::Library::shuffle( groups.keys(), groups );

    yue::bell::MediaCtrlBase::instance()->setCurrentPlaylist(lst);
    //auto pl = yue::bell::PlaylistManager::instance()->openCurrent();
    //pl->set(lst);

    //
    return true;
}

void LibraryTreeListModel::search(QString query)
{
    try {
        QList<yue::bell::LibraryTreeNode*> forest = yue::bell::Library::instance()->queryToForest(query);
        setNewData(forest);
        m_lastError = "";
    } catch (yue::core::ParseError& e) {
        m_lastError = e.what();
        qWarning() << m_lastError;

    } catch (...) {
        m_lastError = "Unhandled exception";
        qWarning() << m_lastError;

    }

    // error status changes wheter an error occured or not.
    emit errorStatusChanged();
}

void LibraryTreeListModel::collectSelectedSongs(QMap<yue::bell::Database::uid_t, QString>& groups, yue::bell::LibraryTreeNode* node )
{
    if (node->numChildren()==0) {
        // get the uid for the leaf node (a song)
        // and the get the text for the grandparent (an artist)
        if (node->getCheckState()!=Qt::Unchecked)
            groups[node->getUid()] = node->parent()->parent()->text();
    } else {
        for (yue::bell::LibraryTreeNode* child : node->children()) {
            collectSelectedSongs(groups,child);
        }
    }
}

QVariant LibraryTreeListModel::data(const QModelIndex &index, int role/* = Qt::DisplayRole*/) const
{
    if (index.row() < 0 || index.row() >= rowCount())
        return QVariant();

    if (role == LibraryTreeListModel::UniqueIdRole)
        return m_tabledata[index.row()]->getUid();

    return TreeListModelBase::data(index,role);
}

QHash<int, QByteArray> LibraryTreeListModel::roleNames() const
{

    QHash<int, QByteArray> roles(TreeListModelBase::roleNames());

    roles[LibraryTreeListModel::UniqueIdRole] = "uid";

    return roles;
}

void LibraryTreeListModel::onDefaultQueryChanged()
{
    search(m_defaultQuery);
}

} // qtcommon
} // yue
