#ifndef UI_PAGE_QUEUE_H
#define UI_PAGE_QUEUE_H

#include <QScopedPointer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QProgressBar>
#include <QLabel>
#include <QListView>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QVariant>
#include <QDialog>
#include <QPushButton>

#include "yue/qtcommon/PlaylistModel.h"
#include "yue/qtcommon/gesture.h"

class PlaylistDelegate: public QStyledItemDelegate
{
    Q_OBJECT

    QIcon m_icoMore;

    int m_currentIndex;
public:
    PlaylistDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
        , m_icoMore(QIcon(":/res/more.svg"))
    {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;


    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setCurrentIndex(int index) {m_currentIndex = index;}
};

class PlaylistView : public QListView
{
    Q_OBJECT
private:
    yue::qtcommon::Gesture m_gesture;
    yue::qtcommon::PlaylistModel *m_model;
    PlaylistDelegate *m_delegate;


signals:
    void more(QModelIndex index);

public:
    PlaylistView(QWidget *parent = nullptr);
    ~PlaylistView() {}

    virtual void mousePressEvent(QMouseEvent *event) {
        m_gesture.mousePressEvent(event);
    }

    virtual void mouseMoveEvent(QMouseEvent *event) {
        m_gesture.mouseMoveEvent(event);
    }

    virtual void mouseReleaseEvent(QMouseEvent *event) {
        m_gesture.mouseReleaseEvent(event);
    }

    /*
    virtual void mouseReleaseEvent(QMouseEvent *event) {
        const QModelIndex index = indexAt(event->pos());
        const QSize size = sizeHintForIndex(index);

        if (event->pos().x() > (viewport()->width() - size.height())) {
            emit more(index);
        } else {
            QListView::mouseReleaseEvent(event);
        }
    }
    */

    void refresh();
    void setCurrentIndex(int index) {
        m_delegate->setCurrentIndex(index);
        viewport()->repaint();
        qDebug() << "update" << index;
    }

    void move(int src, int dst);
    void remove(int index);
};

class QueueMoreDialog : public QDialog
{
    Q_OBJECT

    int m_index;

    QVBoxLayout *m_layoutCentral;
    QPushButton *m_btnPlay;
    QPushButton *m_btnPlayNext;
    QPushButton *m_btnRemove;
    QPushButton *m_btnCancel;

public:
    QueueMoreDialog(int row, int currentIndex, QWidget *parent=nullptr);

signals:
    void play(int index);
    void playNext(int index);
    void remove(int index);

public slots:
    void onPlay();
    void onPlayNext();
    void onRemove();
    void onCancel();

};

namespace UI {

class uiPageQueue;

} // namespace UI

class PageQueue : public QWidget
{
    Q_OBJECT

private:

public:

    PageQueue(QWidget *parent = nullptr);
    ~PageQueue();

public slots:
    void onMore(QModelIndex index);
    void onPlaylistChanged();

    void onCurrentIndexChanged(int index);

    void onPlay(int index);
    void onPlayNext(int index);
    void onRemove(int index);

private:
    QScopedPointer<UI::uiPageQueue> m_ui;
};

#endif
