
#include "ui/page_library.h"
#include "yue/qtcommon/iconbutton.h"
#include "yue/bell/MediaCtrlBase.h"

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
    int rating = index.data(yue::qtcommon::TreeListModelBase::RatingRole).toInt();

    if (check/*option.state & QStyle::State_Selected*/) {
        QBrush color = option.palette.highlight();
        if (check == Qt::CheckState::PartiallyChecked)
            color = option.palette.highlight().color().lighter();
        painter->fillRect(option.rect, color);
        painter->setPen(Qt::white);
        painter->setBrush(option.palette.highlightedText());
    }
    else
    {
        painter->setPen(QPen(option.palette.foreground(), 0));
        painter->setBrush(qvariant_cast<QBrush>(index.data(Qt::ForegroundRole)));
    }

    int height = option.rect.height();
    if (height%2 == 0) {
        height -= 1;
    }

    int left = option.rect.left();
    int top = option.rect.top();

    int xoffset=0;

    painter->save();

    if (count>0) {
        xoffset+=1;
        int h = static_cast<int>(.75F * height);
        int w = h - (h/3) - (h/3);
        int s = (option.rect.height() - h) / 2;
        // draw the expansion state
        if (!expand) {
            QRect vrect(left + h/3 + (depth*height) + s, top + s, w, h);
            painter->fillRect(vrect, QBrush(QColor(0,0,0)));
        }
        QRect hrect(left + (depth*height) + s, top + h/3 + s, h, w);
        painter->fillRect(hrect, QBrush(QColor(0,0,0)));

        // paint the bounding box to check centering
        //painter->drawRect(option.rect.left(), option.rect.top(),
        //                  option.rect.height(), option.rect.height());
    }

    QRect rect = option.rect;
    rect.setLeft(left + ((depth + xoffset)*height));
    int w = 3 * rect.height() / 4;
    rect.setRight(rect.right() - w);

    QRect rectBtn(rect.right() + (1*w/10),
                  rect.top() + (2*rect.height()/10),
                  (8*w/10),
                  (6*rect.height()/10));
    QString text = index.data(Qt::DisplayRole).toString();

    painter->drawText(rect, Qt::AlignVCenter|Qt::AlignLeft, text);

    painter->restore();

    if (depth == 2) {
        painter->save();

        QPainterPath path;
        path.addRoundRect(rectBtn, w/2, w/2);
        painter->setPen(QPen(Qt::black, 2));
        painter->fillPath(path, (rating>=8)?Qt::yellow:Qt::gray);
        painter->drawPath(path);

        painter->restore();
    }


}

QSize LibraryTreeDelegate::sizeHint(
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

    m_gesture.setViewport(this);
    m_gesture.setVScrollBar(this->verticalScrollBar());

    connect(&m_gesture, &yue::qtcommon::Gesture::tap,
            this, &LibraryView::onTap);

    m_delegate = new LibraryTreeDelegate(this);
    m_model = new yue::qtcommon::LibraryTreeListModel(this);

    this->setModel(m_model);
    this->setItemDelegate(m_delegate);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::NoSelection);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

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

void LibraryView::toggleExpand()
{
    if (m_model->anyExpanded()) {
        m_model->expandAll(false);
    } else {
        m_model->expandAll(true);
    }
}

void LibraryView::createPlaylist(bool shuffle)
{
    m_model->createPlaylist(shuffle);
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
    yue::qtcommon::IconButton *m_btnClearSearch;
    yue::qtcommon::IconButton *m_btnSearch;

    yue::qtcommon::IconButton *m_btnSpeech;
    yue::qtcommon::IconButton *m_btnToggleSelection;
    yue::qtcommon::IconButton *m_btnToggleExpand;
    yue::qtcommon::IconButton *m_btnToggleShuffle;
    yue::qtcommon::IconButton *m_btnCreatePlaylist;

    LibraryView *m_view;

    uiPageLibrary(QWidget *parent = nullptr);
    ~uiPageLibrary();

};

uiPageLibrary::uiPageLibrary(QWidget *parent) {
    m_layoutCentral = new QVBoxLayout();
    m_layoutSearch = new QHBoxLayout();
    m_layoutCreate = new QHBoxLayout();

    m_editSearch = new QLineEdit(parent);

    m_btnClearSearch = new yue::qtcommon::IconButton(QIcon(":/res/clear.svg"), parent);
    m_btnSpeech = new yue::qtcommon::IconButton(QIcon(":/res/speech.svg"), parent);
    m_btnToggleSelection = new yue::qtcommon::IconButton(QIcon(":/res/select.svg"), parent);
    m_btnToggleExpand = new yue::qtcommon::IconButton(QIcon(":/res/select.svg"), parent);
    m_btnToggleShuffle = new yue::qtcommon::IconButton(QIcon(":/res/shuffle.svg"), parent);
    m_btnCreatePlaylist = new yue::qtcommon::IconButton(QIcon(":/res/playlist.svg"), parent);

    m_view = new LibraryView(parent);

    m_layoutSearch->addWidget(m_editSearch);
    m_layoutSearch->addWidget(m_btnClearSearch);

    m_layoutCreate->addWidget(m_btnSpeech);
    m_layoutCreate->addWidget(m_btnToggleSelection);
    m_layoutCreate->addWidget(m_btnToggleExpand);
    m_layoutCreate->addWidget(m_btnToggleShuffle);
    m_layoutCreate->addWidget(m_btnCreatePlaylist);

    m_layoutCentral->addLayout(m_layoutSearch);
    m_layoutCentral->addWidget(m_view);
    m_layoutCentral->addLayout(m_layoutCreate);

    m_btnToggleExpand->setVisible(false);

    m_editSearch->setPlaceholderText("Search Library");
    parent->setLayout(m_layoutCentral);

}

uiPageLibrary::~uiPageLibrary() {
}

} // namespace UI

PageLibrary::PageLibrary(QWidget *parent)
    : QWidget(parent)
    , m_ui(new UI::uiPageLibrary(this))
    , m_shuffle(true)
{
    connect(m_ui->m_editSearch, &QLineEdit::editingFinished,
            this, &PageLibrary::onEditingFinished);

    connect(m_ui->m_btnToggleSelection, &yue::qtcommon::IconButton::clicked,
            this, &PageLibrary::onToggleSelection);

    connect(m_ui->m_btnToggleExpand, &yue::qtcommon::IconButton::clicked,
            this, &PageLibrary::onToggleExpand);

    connect(m_ui->m_btnCreatePlaylist, &yue::qtcommon::IconButton::clicked,
            this, &PageLibrary::onCreatePlaylist);

    connect(m_ui->m_btnToggleShuffle, &yue::qtcommon::IconButton::clicked,
            this, &PageLibrary::onToggleShuffle);

    connect(m_ui->m_btnClearSearch, &yue::qtcommon::IconButton::clicked,
            this, &PageLibrary::onClearSearch);

    connect(m_ui->m_btnSpeech, &yue::qtcommon::IconButton::clicked,
            this, &PageLibrary::onSpeechClicked);

}

PageLibrary::~PageLibrary() {
}

void PageLibrary::onEditingFinished()
{
    m_ui->m_view->setQuery(m_ui->m_editSearch->text());
}

void PageLibrary::onToggleSelection()
{
    m_ui->m_view->toggleChecked();
}

void PageLibrary::onToggleExpand()
{
    m_ui->m_view->toggleExpand();
}

void PageLibrary::onToggleShuffle()
{
    m_shuffle = !m_shuffle;
    m_ui->m_btnToggleShuffle->setIcon(QIcon(m_shuffle?":/res/shuffle.svg":":/res/sort.svg"));
}


void PageLibrary::onCreatePlaylist()
{
    m_ui->m_view->createPlaylist(m_shuffle);
}

void PageLibrary::onClearSearch()
{
    m_ui->m_editSearch->setText("");
    m_ui->m_view->setQuery("");
}

void PageLibrary::setQuery(QString query)
{
    m_ui->m_editSearch->setText(query);
    m_ui->m_view->setQuery(query);
}

void PageLibrary::onSpeechClicked()
{
    auto inst = yue::bell::MediaCtrlBase::instance();
    inst->tts();
}
