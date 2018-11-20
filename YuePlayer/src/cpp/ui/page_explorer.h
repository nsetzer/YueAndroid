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
#include "yue/qtcommon/gesture.h"

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
    yue::qtcommon::Gesture m_gesture;
    yue::qtcommon::DirectoryListModel *m_model;
    ExplorerDelegate *m_delegate;

public:
    ExplorerView(QWidget *parent = nullptr);
    ~ExplorerView() {}

    void openParentDirectory();
    void openRoot();

    QString currentDirectory();

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

    void onTap(int x, int y);

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
    void onScan();
    void onScanUpdate(bool running, int ndirs, int nfiles, int nsongs);
private:
    QScopedPointer<UI::uiPageExplorer> m_ui;
    bool m_scanRunning;
};


#endif
