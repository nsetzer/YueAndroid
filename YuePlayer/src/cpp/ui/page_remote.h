#ifndef UI_PAGE_REMOTE_H
#define UI_PAGE_REMOTE_H

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
#include "yue/bell/MediaCtrlBase.h"

class RemoteTreeDelegate: public QStyledItemDelegate
{
    Q_OBJECT

public:
    RemoteTreeDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};


class RemoteView : public QListView
{
    Q_OBJECT
private:
    yue::qtcommon::Gesture m_gesture;
    yue::qtcommon::RemoteTreeListModel *m_model;
    RemoteTreeDelegate *m_delegate;

public:
    RemoteView(QWidget *parent = nullptr);
    virtual ~RemoteView() override {}

    void setQuery(QString query);
    void toggleChecked();
    void toggleExpand();
    void createPlaylist(bool shuffle);

protected:

    virtual void mousePressEvent(QMouseEvent *event) override {
        m_gesture.mousePressEvent(event);
    }

    virtual void mouseMoveEvent(QMouseEvent *event) override {
        m_gesture.mouseMoveEvent(event);
    }

    virtual void mouseReleaseEvent(QMouseEvent *event) override {
        m_gesture.mouseReleaseEvent(event);
    }

    virtual void onTap(int x, int y) {

        const QModelIndex index = indexAt(QPoint(x, y));

        if (!index.isValid()) {
            return;
        }
        const QSize size = sizeHintForIndex(index);

        yue::bell::Database::uid_t songId = index.data(
            yue::qtcommon::TreeListModelBase::SongIdRole).toString();
        int depth = index.data(yue::qtcommon::TreeListModelBase::DepthRole).toInt();
        int count = index.data(yue::qtcommon::TreeListModelBase::ChildCountRole).toInt();
        int rating = index.data(yue::qtcommon::TreeListModelBase::RatingRole).toInt();

        int x1 = (1+depth)*size.height();
        int x2 = viewport()->width() - (3*size.height()/4);

        if (depth < 2 && count>0 && x < x1) {
            onDoubleClick(index);
        } else if (depth == 2 && x < x1) {
            int tmp = (rating>=0)?1:0;
            m_model->setData(index, tmp, yue::qtcommon::TreeListModelBase::RatingRole);
        } else if (depth == 2 && x > x2) {
            auto inst = yue::bell::MediaCtrlBase::instance();
            qDebug() << "adding " << songId << "to playlist";
            inst->playNext(songId);
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

class uiPageRemote;

} // namespace UI

class PageRemote : public QWidget
{
    Q_OBJECT

public:

    PageRemote(QWidget *parent = nullptr);
    ~PageRemote();

public slots:
    void setQuery(QString query);

private slots:
    void onEditingFinished();
    void onToggleSelection();
    void onToggleExpand();
    void onToggleShuffle();
    void onCreatePlaylist();
    void onClearSearch();
    void onSpeechClicked();

private:
    QScopedPointer<UI::uiPageRemote> m_ui;
    bool m_shuffle;
};

#endif
