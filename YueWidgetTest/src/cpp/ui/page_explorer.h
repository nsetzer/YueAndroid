#ifndef UI_PAGE_EXPLORER_H
#define UI_PAGE_EXPLORER_H

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
#include <QMouseEvent>

#include "yue/qtcommon/DirectoryListModel.h"

class ExplorerDelegate: public QStyledItemDelegate
{
    Q_OBJECT

public:
    ExplorerDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;


    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class ExplorerView : public QListView
{
    Q_OBJECT
private:
    yue::qtcommon::DirectoryListModel *m_model;
    ExplorerDelegate *m_delegate;

public:
    ExplorerView(QWidget *parent = nullptr);
    ~ExplorerView() {}

    void openParentDirectory();

protected:
    virtual void mouseReleaseEvent(QMouseEvent *event);

};

namespace UI {

class uiPageExplorer;

} // namespace UI

class PageExplorer : public QWidget
{
    Q_OBJECT

private:

public:

    PageExplorer(QWidget *parent = nullptr);
    ~PageExplorer();

public slots:
    void onOpenParentDir(bool checked=false);
private:
    QScopedPointer<UI::uiPageExplorer> m_ui;
};


#endif
