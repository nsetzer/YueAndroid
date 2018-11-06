

#include "ui/page_explorer.h"

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

    painter->drawRect(option.rect.x()+2, option.rect.y()+2, option.rect.height() - 4, option.rect.height() - 4);
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
    QHBoxLayout *m_layoutNav;
    QToolButton *m_btnBack;
    ExplorerView *m_view;



    uiPageExplorer(QWidget *parent = nullptr);
    ~uiPageExplorer();

};


uiPageExplorer::uiPageExplorer(QWidget *parent)
{
    m_layoutCentral = new QVBoxLayout();

    m_layoutNav = new QHBoxLayout();
    m_btnBack = new QToolButton(parent);

    m_view = new ExplorerView(parent);

    m_layoutNav->addWidget(m_btnBack);
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

    connect(m_ui->m_btnBack, &QToolButton::clicked,
            this, &PageExplorer::onOpenParentDir);
}

PageExplorer::~PageExplorer() {
}

void PageExplorer::onOpenParentDir(bool checked)
{
    Q_UNUSED(checked);
    m_ui->m_view->openParentDirectory();
}
