
#include <QOperatingSystemVersion>
#include <QDesktopServices>
#include <QUrl>

#include "yue/qtcommon/DirectoryListModel.hpp"

namespace yue {
namespace qtcommon {

int DirectoryListModel::IconRole = Qt::UserRole+1;
int DirectoryListModel::IsDirectoryRole = Qt::UserRole+2;

DirectoryListModel::DirectoryListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // dont subclass, create a new model
    // with items in the list
    // add user data roles, roleNAmes for Icon
    //if (QOperatingSystemVersion.type() == QOperatingSystemVersion::Windows)

    if (QOperatingSystemVersion::current().type() == QOperatingSystemVersion::Windows)
        changeDirectory("C:\\");
    else
        changeDirectory("/");
}

int DirectoryListModel::rowCount(const QModelIndex &parent/* = QModelIndex()*/) const
{
    Q_UNUSED(parent);
    return m_tabledata.size();
}

int DirectoryListModel::columnCount(const QModelIndex &parent/* = QModelIndex()*/) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant DirectoryListModel::data(const QModelIndex &index, int role/* = Qt::DisplayRole*/) const
{


    if (index.row() < 0 || index.row() >= rowCount())
        return QVariant();

    if (role == Qt::DisplayRole)
        return m_tabledata[index.row()].fileName();

    if (role == DirectoryListModel::IsDirectoryRole)
        return m_tabledata[index.row()].isDir();

    if (role == DirectoryListModel::IconRole)
    {
        QString path = iconPath( index.row() );
        return path;
    }

    return QVariant();
}

bool DirectoryListModel::setData(const QModelIndex &index, const QVariant &value, int role/* = Qt::EditRole*/)
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


QHash<int, QByteArray> DirectoryListModel::roleNames() const
{

    QHash<int, QByteArray> roles(QAbstractItemModel::roleNames());

    //roles[Qt::DisplayRole] = "display";
    //roles[Qt::UserRole+1] = "icon";
    roles[DirectoryListModel::IconRole] = "icon";
    roles[DirectoryListModel::IsDirectoryRole] = "isdir";
    return roles;
}
/*
void DirectoryListModel::setNewData(QStringList lst)
{
    emit beginResetModel();
    m_tabledata = lst;
    emit endResetModel();
}*/

void DirectoryListModel::openParentDirectory()
{
    // get the parent directory path
    QString path = QFileInfo(m_pwd.absolutePath()).absolutePath();
    qWarning(qPrintable(path));
    changeDirectory(path);
}

void DirectoryListModel::changeDirectory(int index)
{
    if (m_tabledata[index].isDir())
    {
        qWarning( "%d: %s", index, qPrintable(m_tabledata[index].absoluteFilePath()) );
        changeDirectory( m_tabledata[index].absoluteFilePath() );
    }

    return;
}

void DirectoryListModel::changeDirectory(QString path)
{
    m_pwd = QDir(path);
    emit currentDirectoryChanged(path);
    emit beginResetModel();
    //m_tabledata = m_pwd.entryInfoList();
    qWarning("load directory: %s",qPrintable(path));

    m_selected.clear();
    m_tabledata = m_pwd.entryInfoList(QDir::NoDotAndDotDot| QDir::AllEntries,
                                      QDir::DirsFirst|QDir::Name);

    // hack for android
    if (path == "/" && m_tabledata.size()==0)
    {
        {
            QFileInfo fi(QDir::currentPath());
            if (fi.permission(QFile::ExeGroup))
            {
                m_tabledata.push_back(fi);
            }
        }
        {
            QFileInfo fi("/mnt");
            if (fi.permission(QFile::ExeGroup))
            {
                m_tabledata.push_back(fi);
            }
        }

    }
    qWarning("table size: %d",m_tabledata.size());
    emit endResetModel();
    emit rowCountChanged(m_tabledata.size());
}

void DirectoryListModel::openFile(int index)
{
    if (m_tabledata[index].isFile())
    {
        QString path = m_tabledata[index].absoluteFilePath();
        QUrl url = QUrl::fromLocalFile(path);
        QDesktopServices::openUrl( url );
    }
}

QString DirectoryListModel::getCurrentDirectoryName( void ) const
{
    QString name = m_pwd.dirName();
    if (name.size()==0)
        return "root";
    return name;
}

bool DirectoryListModel::isSelected(int index)
{
    return m_selected.contains(index);
}

void DirectoryListModel::toggleSelection(int index)
{
    if (m_selected.contains(index))
        m_selected.remove(index);
    else
        m_selected.insert(index);
}

QString DirectoryListModel::iconPath( int index ) const
{
    QFileInfo fi = m_tabledata[index];
    if (fi.isDir())
        return "images/00_folder.svg";

    QSet<QString> images;
    images << "jpg" << "png" << "bmp";
    if (images.contains(fi.suffix()))
        return QUrl::fromLocalFile(fi.absoluteFilePath()).toString();

    QSet<QString> songs;
    songs << "wav" << "mp3" << "flac";
    if (songs.contains(fi.suffix()))
        return "images/00_file_song.svg";


    return "images/00_file_default.svg";
}

} // qtcommon
} // yue
