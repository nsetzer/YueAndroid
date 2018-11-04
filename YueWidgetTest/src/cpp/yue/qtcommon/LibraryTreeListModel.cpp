
#include <QDir>
#include <QDebug>
#include "yue/qtcommon/LibraryTreeListModel.hpp"
#include "yue/bell/MediaCtrlBase.h"
#include <QtConcurrent>

namespace yue {
namespace qtcommon {

int LibraryTreeListModel::UniqueIdRole = LibraryTreeListModel::UserRole + 1;

LibraryTreeListModel::LibraryTreeListModel(QObject *parent)
    : TreeListModelBase(parent)
{

    connect(this,&LibraryTreeListModel::defaultQueryChanged,
            this,&LibraryTreeListModel::onDefaultQueryChanged);

    connect(this,&LibraryTreeListModel::searchResult,
            this,&LibraryTreeListModel::setNewData);

}

/**
 * @brief LibraryTreeListModel::createPlaylist
 * @return true when a playlist is created
 */
bool LibraryTreeListModel::createPlaylist(bool shuffle/* = true*/)
{
    LOG_FUNCTION_TIME();

    bool success = false;

    QML_EXCEPTION_GUARD_BEGIN

    QMap<yue::bell::Database::uid_t, QString> groups;

    for (yue::bell::LibraryTreeNode* node : m_forest)
    {
        collectSelectedSongs(groups,node);
    }
    if (groups.size()==0) {
        return false;
    }

    QList<yue::bell::Database::uid_t> lst;
    if (shuffle) {
        lst = yue::bell::Library::shuffle( groups.keys(), groups );
    } else {
        lst = groups.keys();
        yue::bell::Library::instance()->sort( lst );
    }

    yue::bell::MediaCtrlBase::instance()->setCurrentPlaylist(lst, true);
    //yue::bell::MediaCtrlBase::instance()->playSong(0);
    success = true;

    QML_EXCEPTION_GUARD_END

    return success;
}

void LibraryTreeListModel::search(QString query)
{
    //QtConcurrent::run(this,&LibraryTreeListModel::searchImpl, query);
    // TODO: move this to a thread, but open a new database connection in that thread
    // http://doc.qt.io/qt-5/qsqldatabase.html#setDatabaseName
    searchImpl(query);
}

void LibraryTreeListModel::searchImpl(QString query) {
    try {
        QList<yue::bell::LibraryTreeNode*> forest = yue::bell::Library::instance()->queryToForest(query);
        emit searchResult( forest );
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

    if (role == LibraryTreeListModel::UniqueIdRole) {
        return bell::toQVariant(m_tabledata[index.row()]->getUid());
    }

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
