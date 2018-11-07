#ifndef UI_PAGE_LIBRARY_H
#define UI_PAGE_LIBRARY_H

#include <QLineEdit>
#include <QListView>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QVariant>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>

#include "yue/qtcommon/TreeListModelBase.h"
#include "yue/qtcommon/LibraryTreeListModel.h"

class LibraryTreeDelegate: public QStyledItemDelegate
{
    Q_OBJECT

public:
    LibraryTreeDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};


class LibraryView : public QListView
{
    Q_OBJECT
private:
    yue::qtcommon::LibraryTreeListModel *m_model;
    LibraryTreeDelegate *m_delegate;

public:
    LibraryView(QWidget *parent = nullptr);
    ~LibraryView() {}

    void setQuery(QString query);
    void toggleChecked();
    void createPlaylist();

protected:

    virtual void mouseReleaseEvent(QMouseEvent *event) {
        const QModelIndex index = indexAt(event->pos());
        const QSize size = sizeHintForIndex(index);
        int depth = index.data(yue::qtcommon::TreeListModelBase::DepthRole).toInt();
        int count = index.data(yue::qtcommon::TreeListModelBase::ChildCountRole).toInt();

        if (count>0 && event->pos().x() < (1+depth)*size.height()) {
            onDoubleClick(index);
        } else {
            QListView::mouseReleaseEvent(event);
        }
    }

public slots:

    void onDoubleClick(const QModelIndex &index) {
        m_model->toggleExpandItem(index.row());
        return;
    }

    void onClick(const QModelIndex &index) {
        m_model->toggleCheckState(index.row());
        return;
    }

};

namespace UI {

class uiPageLibrary;

} // namespace UI

class PageLibrary : public QWidget
{
    Q_OBJECT

public:

    PageLibrary(QWidget *parent = nullptr);
    ~PageLibrary();

protected:
    void onEditingFinished();
    void onToggleSelection(bool checked);
    void onCreatePlaylist(bool checked);

private:
    QScopedPointer<UI::uiPageLibrary> m_ui;
};

#endif // UI_PAGE_LIBRARY_H
