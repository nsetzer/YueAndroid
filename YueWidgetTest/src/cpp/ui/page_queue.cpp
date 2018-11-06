

#include "ui/page_queue.h"

void PlaylistDelegate::paint(
    QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    painter->save();

    QSize song_id = index.data(yue::qtcommon::PlaylistModel::SongIdRole).toSize();
    int song_index = index.data(yue::qtcommon::PlaylistModel::IndexRole).toInt();
    QString song_artist = index.data(yue::qtcommon::PlaylistModel::ArtistRole).toString();
    QString song_album = index.data(yue::qtcommon::PlaylistModel::AlbumRole).toString();
    QString song_title = index.data(yue::qtcommon::PlaylistModel::TitleRole).toString();
    int song_length = index.data(yue::qtcommon::PlaylistModel::LengthRole).toInt();

    int _s = song_length % 60;
    int _m = song_length / 60;
    int _h = _m / 60;
    _m = _m % 60;

    QString song_duration;
    if (_h > 0) {
        song_duration = QString("%1:%2:%3 ")
                .arg(_h)
                .arg(_h, 2, 10, QChar('0'))
                .arg(_s, 2, 10, QChar('0'));
    } else {
        song_duration = QString("%2:%3 ")
                .arg(_m)
                .arg(_s, 2, 10, QChar('0'));
    }

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

    QFont font = option.font;
    QFontMetrics fm(font);

    // create a rect covering the top half of the area
    QRect rectTop = option.rect;
    rectTop.setBottom(option.rect.top() + option.rect.height()/2);
    rectTop.setLeft(rectTop.left()+option.rect.height());
    // create a rect covering the bottom half of the area
    QRect rectBottomTime = option.rect;
    rectBottomTime.setTop(option.rect.top() + option.rect.height()/2);
    rectBottomTime.setLeft(rectBottomTime.left()+option.rect.height());
    // leave space on the RHS for the duration
    QRect rectBottom = rectBottomTime;
    rectBottom.setRight(rectBottom.right() - 6 * fm.averageCharWidth());

    painter->drawRect(option.rect.x()+2, option.rect.y()+2, option.rect.height() - 4, option.rect.height() - 4);
    font.setPixelSize(static_cast<int>(option.font.pixelSize() * 1.1));
    painter->setFont(font);
    painter->drawText(rectTop, Qt::AlignVCenter|Qt::AlignLeft, song_title);
    font.setPixelSize(static_cast<int>(option.font.pixelSize() * 0.8));
    painter->setFont(font);
    painter->drawText(rectBottom, Qt::AlignVCenter|Qt::AlignLeft, song_artist);
    painter->setFont(font);
    painter->drawText(rectBottomTime, Qt::AlignVCenter|Qt::AlignRight, song_duration);

    painter->restore();
}

QSize PlaylistDelegate::sizeHint(
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{

    QFontMetrics fm(option.font);
    fm.height();
    int w = -1;
    int h = 3*fm.height();
    return QSize(w, h);
}

PlaylistView::PlaylistView(QWidget *parent)
  : QListView(parent)
{
    m_model = new yue::qtcommon::PlaylistModel(this);
    m_delegate = new PlaylistDelegate(this);

    this->setModel(m_model);
    this->setItemDelegate(m_delegate);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::NoSelection);

    m_model->setPlaylistName(":current:");
    m_model->refresh();
}


namespace UI {

class uiPageQueue
{

public:

    QVBoxLayout *m_layoutCentral;
    PlaylistView *m_view;


    uiPageQueue(QWidget *parent = nullptr);
    ~uiPageQueue();

};


uiPageQueue::uiPageQueue(QWidget *parent)
{
    m_layoutCentral = new QVBoxLayout();

    m_view = new PlaylistView(parent);

    parent->setLayout(m_layoutCentral);
    m_layoutCentral->addWidget(m_view);
}

uiPageQueue::~uiPageQueue()
{

}

} // namespace ui


PageQueue::PageQueue(QWidget *parent)
    : QWidget(parent)
    , m_ui(new UI::uiPageQueue(this)) {

}

PageQueue::~PageQueue() {
}
