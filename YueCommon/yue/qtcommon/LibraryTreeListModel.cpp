
#include <QDir>
#include <QDebug>
#include "yue/qtcommon/LibraryTreeListModel.hpp"


namespace yue {
namespace qtcommon {

LibraryTreeListModel::LibraryTreeListModel(QObject *parent)
    : TreeListModelBase(parent)
{

    QList<yue::bell::LibraryTreeNode*> forest = yue::bell::Library::instance()->queryToForest("");
    setNewData(forest);

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


    auto pl = yue::bell::PlaylistManager::instance()->openCurrent();
    pl->set(lst);

    //
    return true;
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

} // qtcommon
} // yue
