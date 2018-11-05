#ifndef UI_PAGE_LIBRARY_H
#define UI_PAGE_LIBRARY_H

#include <QLineEdit>
#include <QGesture>
#include <QListView>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QVariant>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>

#include "yue/qtcommon/TreeListModelBase.hpp"
#include "yue/qtcommon/LibraryTreeListModel.hpp"

class TestDelegate: public QStyledItemDelegate
{
    Q_OBJECT

public:
    TestDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;
};


class LibraryView : public QListView
{
    Q_OBJECT
private:
    yue::qtcommon::LibraryTreeListModel *m_model;
    TestDelegate *m_delegate;

public:
    LibraryView(QWidget *parent = nullptr);
    ~LibraryView() {}

protected:
    virtual void mouseReleaseEvent(QMouseEvent *event) {
        const QModelIndex index = indexAt(event->pos());
        const QSize size = sizeHintForIndex(index);
        int depth = index.data(yue::qtcommon::TreeListModelBase::DepthRole).toInt();
        int count = index.data(yue::qtcommon::TreeListModelBase::ChildCountRole).toInt();
        qDebug() << size.height() << event->pos();

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

    bool event(QEvent *event)
    {
        if (event->type() == QEvent::Gesture) {
            return gestureEvent(static_cast<QGestureEvent*>(event));
        }
        return QWidget::event(event);
    }

    bool gestureEvent(QGestureEvent *event)
    {
        qDebug() << "gestureEvent():" << event;
        if (QGesture *swipe = event->gesture(Qt::SwipeGesture))
            qDebug() << "swipe";
        else if (QGesture *pan = event->gesture(Qt::PanGesture))
            qDebug() << "pan";
        if (QGesture *pinch = event->gesture(Qt::PinchGesture))
            qDebug() << "pinch";
        return true;
    }

};

namespace UI {

class uiPageLibrary;

} // namespace UI

class PageLibrary : public QWidget
{
    Q_OBJECT

private:

public:

    PageLibrary(QWidget *parent = nullptr);
    ~PageLibrary();

private:
    QScopedPointer<UI::uiPageLibrary> m_ui;
};

#endif // UI_PAGE_LIBRARY_H
