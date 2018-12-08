
#include "ui/page_remote.h"
#include "yue/qtcommon/iconbutton.h"


void RemoteTreeDelegate::paint(
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

    int height = option.rect.height();
    if (height%2 == 0) {
        height -= 1;
    }

    int left = option.rect.left();
    int top = option.rect.top();

    int xoffset=0;

    painter->save();

    // construct the 2 rectangles for a plus sign
    int h = static_cast<int>(.75F * height);
    h -= h%2;
    int w = h - (2*h/3);
    int s = (option.rect.height() - h) / 2;
    QRect vrect(left + h/3 + (depth*height) + s, top + s, w, h);
    QRect hrect(left + (depth*height) + s, top + h/3 + s, h, w);
    QRect srect(left + ((depth - 1)*height) + s, top + s, h, h);
    // construct a smaller, inner plus sign
    h -= static_cast<int>(0.25F * h);
    h -= h%2;
    w = h - (2*h/3);
    s = (option.rect.height() - h) / 2;
    QRect vrect2(left + h/3 + (depth*height) + s, top + s, w, h);
    QRect hrect2(left + (depth*height) + s, top + h/3 + s, h, w);

    if (count>0) {
        xoffset+=1;

        // draw the expansion state
        if (!expand) {painter->fillRect(vrect, QBrush(QColor(0,0,0)));}
        painter->fillRect(hrect, QBrush(QColor(0,0,0)));

        if (check == Qt::CheckState::PartiallyChecked) {
            if (!expand) {painter->fillRect(vrect2, QBrush(QColor(0x99,0x99,0x99)));}
            painter->fillRect(hrect2, QBrush(QColor(0x99,0x99,0x99)));
        } else if (check == Qt::CheckState::Unchecked) {
            if (!expand) {painter->fillRect(vrect2, QBrush(QColor(0xEE,0xEE,0xEE)));}
            painter->fillRect(hrect2, QBrush(QColor(0xEE,0xEE,0xEE)));
        }

        // paint the bounding box to check centering
        //painter->drawRect(option.rect.left(), option.rect.top(),
        //                  option.rect.height(), option.rect.height());
    }

    QRect rect = option.rect;
    rect.setLeft(left + ((depth + xoffset)*height));
    int w2 = 3 * rect.height() / 4;
    rect.setRight(rect.right() - w2);

    QRect rectBtn(rect.right() + (1*w2/10),
                  rect.top() + (2*rect.height()/10),
                  (8*w2/10),
                  (6*rect.height()/10));
    QString text = index.data(Qt::DisplayRole).toString();

    painter->drawText(rect, Qt::AlignVCenter|Qt::AlignLeft, text);

    painter->restore();

    if (depth == 2) {
        painter->save();

        // the more button
        QPainterPath path;
        path.addRoundRect(rectBtn, w/2, w/2);
        painter->setPen(QPen(Qt::black, 2));
        painter->fillPath(path, Qt::blue);
        painter->drawPath(path);

        // a button, to the left of the text, for the rating
        QPainterPath path2;
        path2.addRoundRect(srect, w/2, w/2);
        painter->setPen(QPen(Qt::black, 2));
        painter->fillPath(path2, (check != Qt::CheckState::Unchecked)?Qt::yellow:Qt::gray);
        painter->drawPath(path2);

        painter->restore();
    }


}

QSize RemoteTreeDelegate::sizeHint(
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

RemoteView::RemoteView(QWidget *parent)
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
            this, &RemoteView::onTap);

    m_delegate = new RemoteTreeDelegate(this);
    m_model = new yue::qtcommon::RemoteTreeListModel(this);

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

void RemoteView::setQuery(QString query)
{
     m_model->search(query);
}

void RemoteView::toggleChecked()
{
    if (m_model->anySelected()) {
        m_model->checkAll(false);
    } else {
        m_model->checkAll(true);
    }
}

void RemoteView::toggleExpand()
{
    if (m_model->anyExpanded()) {
        m_model->expandAll(false);
    } else {
        m_model->expandAll(true);
    }
}

void RemoteView::createPlaylist(bool shuffle)
{
    m_model->createPlaylist(shuffle);
    m_model->checkAll(false);
}

namespace UI {

class uiPageRemote
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

    RemoteView *m_view;

    uiPageRemote(QWidget *parent = nullptr);
    ~uiPageRemote();

};

uiPageRemote::uiPageRemote(QWidget *parent) {
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

    m_view = new RemoteView(parent);

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

uiPageRemote::~uiPageRemote() {
}

} // namespace UI

PageRemote::PageRemote(QWidget *parent)
    : QWidget(parent)
    , m_ui(new UI::uiPageRemote(this))
    , m_shuffle(true)
{
    connect(m_ui->m_editSearch, &QLineEdit::editingFinished,
            this, &PageRemote::onEditingFinished);

    connect(m_ui->m_btnToggleSelection, &yue::qtcommon::IconButton::clicked,
            this, &PageRemote::onToggleSelection);

    connect(m_ui->m_btnToggleExpand, &yue::qtcommon::IconButton::clicked,
            this, &PageRemote::onToggleExpand);

    connect(m_ui->m_btnCreatePlaylist, &yue::qtcommon::IconButton::clicked,
            this, &PageRemote::onCreatePlaylist);

    connect(m_ui->m_btnToggleShuffle, &yue::qtcommon::IconButton::clicked,
            this, &PageRemote::onToggleShuffle);

    connect(m_ui->m_btnClearSearch, &yue::qtcommon::IconButton::clicked,
            this, &PageRemote::onClearSearch);

    connect(m_ui->m_btnSpeech, &yue::qtcommon::IconButton::clicked,
            this, &PageRemote::onSpeechClicked);

}

PageRemote::~PageRemote() {
}

void PageRemote::onEditingFinished()
{
    m_ui->m_view->setQuery(m_ui->m_editSearch->text());
}

void PageRemote::onToggleSelection()
{
    m_ui->m_view->toggleChecked();
}

void PageRemote::onToggleExpand()
{
    m_ui->m_view->toggleExpand();
}

void PageRemote::onToggleShuffle()
{
    m_shuffle = !m_shuffle;
    m_ui->m_btnToggleShuffle->setIcon(QIcon(m_shuffle?":/res/shuffle.svg":":/res/sort.svg"));
}


void PageRemote::onCreatePlaylist()
{
    m_ui->m_view->createPlaylist(m_shuffle);
}

void PageRemote::onClearSearch()
{
    m_ui->m_editSearch->setText("");
    m_ui->m_view->setQuery("");
}

void PageRemote::setQuery(QString query)
{
    m_ui->m_editSearch->setText(query);
    m_ui->m_view->setQuery(query);
}

void PageRemote::onSpeechClicked()
{
    auto inst = yue::bell::MediaCtrlBase::instance();
    inst->tts();
}
