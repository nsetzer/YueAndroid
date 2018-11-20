#ifndef DIRECTORYLISTMODEL_H
#define DIRECTORYLISTMODEL_H

#include "yue/global.h"
#include <QObject>
#include <QAbstractListModel>
#include <QDir>
#include <QUrl>
#include <QSet>

namespace yue {
namespace qtcommon {

class YUECOMMON_EXPORT DirectoryListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString currentDirectoryName READ getCurrentDirectoryName NOTIFY currentDirectoryChanged)

    Q_PROPERTY(int count READ rowCount NOTIFY rowCountChanged)

    static int IconRole;
    static int IsDirectoryRole;
public:
    explicit DirectoryListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QHash<int, QByteArray> roleNames() const;

    //void setNewData(QStringList lst);

    // --

    Q_INVOKABLE void openParentDirectory();
    void changeDirectory(QString path);
    Q_INVOKABLE void changeDirectory(int index);
    Q_INVOKABLE void openFile(int index);
    QString getCurrentDirectoryName( void ) const;

    Q_INVOKABLE bool isSelected(int index);
    Q_INVOKABLE void toggleSelection(int index);

    QString iconPath( int index ) const;





signals:
    //void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int> ());
    void currentDirectoryChanged(QString path);
    void rowCountChanged(int count);

public slots:

private:
    QDir m_pwd;
    QFileInfoList m_tabledata;
    QSet<int> m_selected;
};

} // qtcommon
} // yue


#endif // DIRECTORYLISTMODEL_H
