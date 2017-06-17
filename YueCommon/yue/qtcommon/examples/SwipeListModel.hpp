#ifndef SwipeListModel_H
#define SwipeListModel_H

#include "yue/global.h"
#include <QObject>
#include <QAbstractListModel>

namespace yue {
namespace qtcommon {

class YUECOMMON_EXPORT SwipeListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SwipeListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    //QHash<int, QByteArray> roleNames() const;

    void setNewData(QStringList lst);

signals:
    //void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int> ());
public slots:

private:
    QStringList m_tabledata;
};


} // qtcommon
} // yue


#endif // BASICLISTMODEL_H
