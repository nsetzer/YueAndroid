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
#include <QPushButton>

#include "yue/qtcommon/TreeListModelBase.h"
#include "yue/qtcommon/LibraryTreeListModel.h"
#include "yue/qtcommon/gesture.h"

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
    yue::qtcommon::Gesture m_gesture;
    yue::qtcommon::LibraryTreeListModel *m_model;
    LibraryTreeDelegate *m_delegate;

public:
    LibraryView(QWidget *parent = nullptr);
    ~LibraryView() {}

    void setQuery(QString query);
    void toggleChecked();
    void toggleExpand();
    void createPlaylist(bool shuffle);

protected:

    virtual void mousePressEvent(QMouseEvent *event) {
        m_gesture.mousePressEvent(event);
    }

    virtual void mouseMoveEvent(QMouseEvent *event) {
        m_gesture.mouseMoveEvent(event);
    }

    virtual void mouseReleaseEvent(QMouseEvent *event) {
        m_gesture.mouseReleaseEvent(event);
    }

    virtual void onTap(int x, int y) {

        const QModelIndex index = indexAt(QPoint(x, y));
        const QSize size = sizeHintForIndex(index);
        int depth = index.data(yue::qtcommon::TreeListModelBase::DepthRole).toInt();
        int count = index.data(yue::qtcommon::TreeListModelBase::ChildCountRole).toInt();

        if (count>0 && x < (1+depth)*size.height()) {
            onDoubleClick(index);
        } else {
            onClick(index);
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

public slots:
    void setQuery(QString query);

private slots:
    void onEditingFinished();
    void onToggleSelection();
    void onToggleExpand();
    void onToggleShuffle();
    void onCreatePlaylist();
    void onClearSearch();

private:
    QScopedPointer<UI::uiPageLibrary> m_ui;
    bool m_shuffle;
};

#endif // UI_PAGE_LIBRARY_H
