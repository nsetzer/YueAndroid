#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QVariant>
#include <QStyledItemDelegate>
#include <QScopedPointer>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTreeView>
#include <QListView>

#include "yue/qtcommon/TreeListModelBase.hpp"
#include "yue/qtcommon/LibraryTreeListModel.hpp"

namespace UI {

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


};

class MainWindow : public QObject
{
    Q_OBJECT

    QWidget *m_centralWidget;
    QVBoxLayout *m_centralLayout;
    QPushButton *m_button;
    LibraryView *m_view;



public:
    explicit MainWindow(QMainWindow *parent = nullptr);
    ~MainWindow();
};

}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QScopedPointer<UI::MainWindow> m_ui;



};

#endif // MAINWINDOW_H
