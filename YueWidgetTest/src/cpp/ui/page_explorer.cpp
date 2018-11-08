

#include "ui/page_explorer.h"

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

    QFontMetrics fm(option.font);
    fm.height();
    int w = -1;
    int h = 2*fm.height();
    return QSize(w, h);
}

ExplorerView::ExplorerView(QWidget *parent)
  : QListView(parent)
{
    m_model = new yue::qtcommon::DirectoryListModel(this);
    m_delegate = new ExplorerDelegate(this);

    this->setModel(m_model);
    this->setItemDelegate(m_delegate);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::NoSelection);

}

void ExplorerView::openParentDirectory()
{
    m_model->openParentDirectory();
}

void ExplorerView::mouseReleaseEvent(QMouseEvent *event)
{
    const QModelIndex index = indexAt(event->pos());
    bool isDir = index.data(yue::qtcommon::DirectoryListModel::IsDirectoryRole).toBool();
    if (isDir) {
        m_model->changeDirectory(index.row());
    } else {
        m_model->openFile(index.row());
    }
}


namespace UI {

class uiPageExplorer
{

public:

    QVBoxLayout *m_layoutCentral;
    //QHBoxLayout *m_layoutNav;
    QToolBar *m_barNav;
    //QToolButton *m_btnHome;
    //QToolButton *m_btnBack;
    ExplorerView *m_view;



    uiPageExplorer(QWidget *parent = nullptr);
    ~uiPageExplorer();

};


uiPageExplorer::uiPageExplorer(QWidget *parent)
{
    m_layoutCentral = new QVBoxLayout();

    //m_layoutNav = new QHBoxLayout();
    m_barNav = new QToolBar();
    //m_btnHome = new QPushButton("home", parent);
    //m_btnBack = new QPushButton("back", parent);

    m_view = new ExplorerView(parent);

    //m_layoutNav->addWidget(m_btnBack);
    //m_layoutNav->addWidget(m_btnHome);
    //m_layoutCentral->addLayout(m_layoutNav);
    m_layoutCentral->addWidget(m_barNav);
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

    //connect(m_ui->m_btnBack, &QToolButton::clicked,
    //        this, &PageExplorer::onOpenParentDir);
    QAction *action;

    action = m_ui->m_barNav->addAction(QIcon(":/res/return.svg"), "");
    connect(action, &QAction::triggered,
            this, &PageExplorer::onOpenParentDir);
    action = m_ui->m_barNav->addAction(QIcon(":/res/return.svg"), "");
}

PageExplorer::~PageExplorer() {
}

void PageExplorer::onOpenParentDir(bool checked)
{
    Q_UNUSED(checked);
    m_ui->m_view->openParentDirectory();
}
