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
#include <QIcon>
#include <QPixmap>
#include <QPushButton>
#include <QToolBar>
#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include <QProgressBar>

#include "yue/qtcommon/DirectoryListModel.h"
#include "yue/qtcommon/iconbutton.h"


class ExplorerDelegate: public QStyledItemDelegate
{
    Q_OBJECT

private:
    QIcon m_icoFile;
    QPixmap m_pixFile;

    QIcon m_icoFolder;
    QPixmap m_pixFolder;
public:
    ExplorerDelegate(QObject *parent = nullptr);

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
    void openRoot();

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
    void onOpenParentDir();
    void onOpenHome();
private:
    QScopedPointer<UI::uiPageExplorer> m_ui;
};


#endif
