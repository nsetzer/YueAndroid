
#include "yue/qtcommon/toolbar.h"
#include "yue/bell/MediaCtrlBase.h"
#include "ui/page_explorer.h"

#include <QApplication>
#include <QLabel>

ExplorerDelegate::ExplorerDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    m_icoFile = QIcon(":/res/file.svg");
    m_icoFolder = QIcon(":/res/folder.svg");
}

void ExplorerDelegate::paint(
    QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{

    QString icon_path = index.data(yue::qtcommon::DirectoryListModel::IconRole).toString();
    bool isDir = index.data(yue::qtcommon::DirectoryListModel::IsDirectoryRole).toBool();
    QString text = index.data(Qt::DisplayRole).toString();

    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
        painter->setPen(Qt::white);
        painter->setBrush(option.palette.highlightedText());
    }
    else
    {
        painter->setPen(QPen(option.palette.foreground(), 0));
        painter->setBrush(qvariant_cast<QBrush>(index.data(Qt::ForegroundRole)));
    }

    QRect rectText = option.rect;

    rectText.setLeft(rectText.left()+option.rect.height());

    int ico_h = static_cast<int>(option.rect.height() * .8);
    int ico_x = (option.rect.height() - ico_h) / 2;
    if (m_pixFile.height() != ico_h) {
        // regenerate the icon for a resized element
        // const cast so that the value can be cached
        const_cast<ExplorerDelegate*>(this)->m_pixFile = m_icoFile.pixmap(QSize(ico_h, ico_h));
        const_cast<ExplorerDelegate*>(this)->m_pixFolder = m_icoFolder.pixmap(QSize(ico_h, ico_h));
    }

    painter->drawRect(option.rect.left() + ico_x, option.rect.top() + ico_x, ico_h, ico_h);
    painter->drawPixmap(option.rect.left() + ico_x, option.rect.top() + ico_x, ico_h, ico_h, (isDir)?m_pixFolder:m_pixFile);
    painter->drawText(rectText, Qt::AlignVCenter|Qt::AlignLeft, text);

}

QSize ExplorerDelegate::sizeHint(
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    Q_UNUSED(index);
    QFontMetrics fm(option.font);
    fm.height();
    int w = -1;
    int h = 2*fm.height();
    return QSize(w, h);
}

ExplorerView::ExplorerView(QWidget *parent)
  : QListView(parent)
  , m_gesture(this)
{
    m_model = new yue::qtcommon::DirectoryListModel(this);
    m_delegate = new ExplorerDelegate(this);

    m_gesture.setViewport(this);
    m_gesture.setVScrollBar(this->verticalScrollBar());

    connect(&m_gesture, &yue::qtcommon::Gesture::tap,
            this, &ExplorerView::onTap);

    this->setModel(m_model);
    this->setItemDelegate(m_delegate);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::NoSelection);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

}

void ExplorerView::openParentDirectory()
{
    m_model->openParentDirectory();
}

void ExplorerView::openRoot()
{
    m_model->openRoot();
}

QString ExplorerView::currentDirectory()
{
    return m_model->currentDirectory();
}

void ExplorerView::onTap(int x, int y)
{
    const QModelIndex index = indexAt(QPoint(x, y));

    if (index.isValid()) {
        bool isDir = index.data(yue::qtcommon::DirectoryListModel::IsDirectoryRole).toBool();
        if (isDir) {
            m_model->changeDirectory(index.row());
        } else {
            m_model->openFile(index.row());
        }
    }

}


namespace UI {

class uiPageExplorer
{

public:

    QVBoxLayout *m_layoutCentral;
    //QHBoxLayout *m_layoutNav;
    QHBoxLayout *m_layoutNav;
    yue::qtcommon::IconButton *m_btnHome;
    yue::qtcommon::IconButton *m_btnBack;
    yue::qtcommon::IconButton *m_btnScan;
    QLabel *m_lblScanStatus;

    ExplorerView *m_view;

    uiPageExplorer(QWidget *parent = nullptr);
    ~uiPageExplorer();

};


uiPageExplorer::uiPageExplorer(QWidget *parent)
{
    m_layoutCentral = new QVBoxLayout();
    m_layoutNav = new QHBoxLayout();

    m_btnBack = new yue::qtcommon::IconButton(QIcon(":/res/return.svg"), parent);
    m_btnHome = new yue::qtcommon::IconButton(QIcon(":/res/home.svg"), parent);
    m_btnScan = new yue::qtcommon::IconButton(QIcon(":/res/scan.svg"), parent);

    m_lblScanStatus = new QLabel("", parent);

    m_view = new ExplorerView(parent);

    m_layoutNav->addWidget(m_btnBack);
    m_layoutNav->addWidget(m_btnHome);
    m_layoutNav->addWidget(m_btnScan);
    m_layoutNav->addWidget(m_lblScanStatus);
    // TODO revist spacing,
    // create a ReactiveUI class to handle desktop sizing
    // no reason for any spacing on desktop
    // spacing is required on android
    m_layoutNav->setSpacing(m_btnBack->width() / 2);

    m_layoutCentral->addLayout(m_layoutNav);
    m_layoutCentral->addWidget(m_view);
    parent->setLayout(m_layoutCentral);
}

uiPageExplorer::~uiPageExplorer()
{

}

} // namespace ui


PageExplorer::PageExplorer(QWidget *parent)
    : QWidget(parent)
    , m_ui(new UI::uiPageExplorer(this))
{

    connect(m_ui->m_btnBack, &yue::qtcommon::IconButton::clicked,
            this, &PageExplorer::onOpenParentDir);

    connect(m_ui->m_btnHome, &yue::qtcommon::IconButton::clicked,
            this, &PageExplorer::onOpenHome);

    connect(m_ui->m_btnScan, &yue::qtcommon::IconButton::clicked,
            this, &PageExplorer::onScan);

    auto inst = yue::bell::MediaCtrlBase::instance();

    connect(inst.data(), &yue::bell::MediaCtrlBase::scanUpdate,
            this, &PageExplorer::onScanUpdate);

    m_scanRunning = false;
    //m_ui->m_btnScan->setEnabled(false);
    inst->scanStatus();

}

PageExplorer::~PageExplorer() {
}

void PageExplorer::onOpenParentDir()
{
    m_ui->m_view->openParentDirectory();
}

void PageExplorer::onOpenHome()
{
    m_ui->m_view->openRoot();
}

void PageExplorer::onScan()
{
    auto inst = yue::bell::MediaCtrlBase::instance();
    if (m_scanRunning) {
        qDebug() << "attempting to stop scan";
        inst->scanStop();
    } else {
        QString path = m_ui->m_view->currentDirectory();
        qDebug() << "starting scan" << path;
        inst->scanStart(path);
    }
}

void PageExplorer::onScanUpdate(bool running, int ndirs, int nfiles, int nsongs)
{
    Q_UNUSED(ndirs);
    Q_UNUSED(nfiles);
    Q_UNUSED(nsongs);

    //m_ui->m_btnScan->setEnabled(true);
    m_scanRunning = running;

    m_ui->m_btnScan->setIcon(QIcon(running?":/res/stop.svg":":/res/scan.svg"));

    m_ui->m_lblScanStatus->setText(running?"Scan Running...":"");
}
