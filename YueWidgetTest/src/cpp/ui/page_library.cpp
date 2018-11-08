
#include "ui/page_library.h"

void LibraryTreeDelegate::paint(
    QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    // https://doc.qt.io/archives/qt-4.8/qstyle.html#StateFlag-enum

    if (!painter->isActive()) {
        return;
    }

    int check = index.data(yue::qtcommon::TreeListModelBase::CheckRole).toInt();
    int expand = index.data(yue::qtcommon::TreeListModelBase::ExpandedRole).toInt();
    int depth = index.data(yue::qtcommon::TreeListModelBase::DepthRole).toInt();
    int count = index.data(yue::qtcommon::TreeListModelBase::ChildCountRole).toInt();

    if (check/*option.state & QStyle::State_Selected*/) {
        painter->fillRect(option.rect, option.palette.highlight());
        painter->setPen(Qt::white);
        painter->setBrush(option.palette.highlightedText());
    }
    else
    {
        painter->setPen(QPen(option.palette.foreground(), 0));
        painter->setBrush(qvariant_cast<QBrush>(index.data(Qt::ForegroundRole)));
    }

    int height = option.rect.height();

    int left = option.rect.left();
    int top = option.rect.top();

    int xoffset=0;
    if (count>0) {
        xoffset+=1;
        int h = static_cast<int>(.75F * height);
        // draw the expansion state
        if (!expand) {
            QRect vrect(left + height/2 - h/2 + (depth*height), top, h/2, h);
            painter->fillRect(vrect, QBrush(QColor(0,0,0)));
        }
        QRect hrect(left + (depth*height), top + height/2 - h/2, h, h/2);
        painter->fillRect(hrect, QBrush(QColor(0,0,0)));
    }

    QRect rect = option.rect;
    rect.setLeft(left + ((depth + xoffset)*height));

    QString text = "(" + index.data(Qt::DisplayRole).toString()
            + ", " + QString::number(check)
            + ", " + QString::number(expand)
            + ", " + QString::number(depth)
            + ", " + QString::number(count) + ")";

    painter->drawText(rect, Qt::AlignVCenter|Qt::AlignLeft, text);
}

QSize LibraryTreeDelegate::sizeHint(
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{

    QFontMetrics fm(option.font);
    fm.height();
    int w = -1;
    int h = 2*fm.height();
    return QSize(w, h);
}

LibraryView::LibraryView(QWidget *parent)
  : QListView(parent)
{

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    #if defined (Q_OS_ANDROID)
       // this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    #endif
    // space at the bottom for better touching the lowest widgets
    // TODO: scale this value to 1x font size
    setContentsMargins(0, 0, 0, 20);

    m_delegate = new LibraryTreeDelegate(this);
    m_model = new yue::qtcommon::LibraryTreeListModel(this);

    this->setModel(m_model);
    this->setItemDelegate(m_delegate);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::NoSelection);

    connect(this ,SIGNAL(doubleClicked(const QModelIndex)),
                this, SLOT(onDoubleClick(const QModelIndex)));

    connect(this ,SIGNAL(clicked(const QModelIndex)),
                this, SLOT(onClick(const QModelIndex)));

    m_model->search("");
}

void LibraryView::setQuery(QString query)
{
     m_model->search(query);
}

void LibraryView::toggleChecked()
{
    if (m_model->anySelected()) {
        m_model->checkAll(false);
    } else {
        m_model->checkAll(true);
    }
}

void LibraryView::createPlaylist()
{
    m_model->createPlaylist();
    m_model->checkAll(false);
}

namespace UI {

class uiPageLibrary
{

public:

    QVBoxLayout *m_layoutCentral;
    QHBoxLayout *m_layoutSearch;
    QHBoxLayout *m_layoutCreate;
    QLineEdit *m_editSearch;
    QToolButton *m_btnSearch;

    QToolButton *m_btnToggleSelection;
    QToolButton *m_btnCreate2;
    QPushButton *m_btnCreatePlaylist;

    LibraryView *m_view;

    uiPageLibrary(QWidget *parent = nullptr);
    ~uiPageLibrary();

};

uiPageLibrary::uiPageLibrary(QWidget *parent) {
    m_layoutCentral = new QVBoxLayout();
    m_layoutSearch = new QHBoxLayout();
    m_layoutCreate = new QHBoxLayout();

    m_editSearch = new QLineEdit(parent);
    m_btnToggleSelection = new QToolButton(parent);
    m_btnCreate2 = new QToolButton(parent);
    m_btnCreatePlaylist = new QPushButton(QIcon(":/res/playlist.svg"), "", parent);

    m_view = new LibraryView(parent);

    m_layoutSearch->addWidget(m_editSearch);

    m_layoutCreate->addWidget(m_btnToggleSelection);
    m_layoutCreate->addWidget(m_btnCreate2);
    m_layoutCreate->addWidget(m_btnCreatePlaylist);

    m_layoutCentral->addLayout(m_layoutSearch);
    m_layoutCentral->addWidget(m_view);
    m_layoutCentral->addLayout(m_layoutCreate);

    m_editSearch->setPlaceholderText("Search Library");
    parent->setLayout(m_layoutCentral);

}

uiPageLibrary::~uiPageLibrary() {
}

} // namespace UI

PageLibrary::PageLibrary(QWidget *parent)
    : QWidget(parent)
    , m_ui(new UI::uiPageLibrary(this))
{
    connect(m_ui->m_editSearch, &QLineEdit::editingFinished,
            this, &PageLibrary::onEditingFinished);

    connect(m_ui->m_btnToggleSelection, &QToolButton::clicked,
            this, &PageLibrary::onToggleSelection);

    connect(m_ui->m_btnCreatePlaylist, &QToolButton::clicked,
            this, &PageLibrary::onCreatePlaylist);
}

PageLibrary::~PageLibrary() {
}

void PageLibrary::onEditingFinished()
{
    m_ui->m_view->setQuery(m_ui->m_editSearch->text());
}

void PageLibrary::onToggleSelection(bool checked)
{
    Q_UNUSED(checked);
    m_ui->m_view->toggleChecked();
}

void PageLibrary::onCreatePlaylist(bool checked)
{
    Q_UNUSED(checked);
    m_ui->m_view->createPlaylist();
}
