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

#include "yue/qtcommon/PlaylistModel.h"

class PlaylistDelegate: public QStyledItemDelegate
{
    Q_OBJECT

public:
    PlaylistDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;


    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class PlaylistView : public QListView
{
    Q_OBJECT
private:
    yue::qtcommon::PlaylistModel *m_model;
    PlaylistDelegate *m_delegate;

public:
    PlaylistView(QWidget *parent = nullptr);
    ~PlaylistView() {}

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

private:
    QScopedPointer<UI::uiPageQueue> m_ui;
};

#endif
