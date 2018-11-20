
#include <QDir>

#include "yue/qtcommon/examples/BasicListModel.hpp"

namespace yue {
namespace qtcommon {

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
BasicListModel::BasicListModel(QObject *parent)
    : QAbstractListModel(parent)
{


}

int BasicListModel::rowCount(const QModelIndex &parent/* = QModelIndex()*/) const
{
    Q_UNUSED(parent);
    return m_tabledata.size();
}

int BasicListModel::columnCount(const QModelIndex &parent/* = QModelIndex()*/) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant BasicListModel::data(const QModelIndex &index, int role/* = Qt::DisplayRole*/) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (index.row() < 0 || index.row() >= rowCount())
        return QVariant();

    return m_tabledata[index.row()];
}

bool BasicListModel::setData(const QModelIndex &index, const QVariant &value, int role/* = Qt::EditRole*/)
{
    if (role != Qt::EditRole)
        return false;

    if (index.row() < 0 || index.row() >= rowCount())
        return false;

    m_tabledata[index.row()] = value.toString();
    emit dataChanged(index,index, {role,});
    return true;
}

void BasicListModel::setNewData(QStringList lst)
{
    emit beginResetModel();
    m_tabledata = lst;
    emit endResetModel();
}

} // qtcommon
} // yue
