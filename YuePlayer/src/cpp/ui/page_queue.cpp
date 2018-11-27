

#include "ui/page_queue.h"
#include "yue/bell/MediaCtrlBase.h"
#include "yue/qtcommon/svg.h"

#include <QSvgRenderer>

void PlaylistDelegate::paint(
    QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    painter->save();

    //QString song_id = index.data(yue::qtcommon::PlaylistModel::SongIdRole).toString();
    //int song_index = index.data(yue::qtcommon::PlaylistModel::IndexRole).toInt();
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

    if (m_currentIndex == index.row() || option.state & QStyle::State_Selected) {
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

    QRect rectMain = option.rect;

    // subtract area for an icon button
    float icon_factor = 0.8F; // total area of icon to take up, including padding
    float icon_factor2 = 0.8F; // percentage of total area, to use for the actual icon
    rectMain.setRight(rectMain.right() - static_cast<int>(icon_factor * rectMain.height()));

    int icon_size = static_cast<int>(icon_factor2 * icon_factor * rectMain.height());
    icon_size = (icon_size/4) * 4;
    // position a square on the right side of the delegate area
    QRect rectIcon(option.rect.right() - icon_size - static_cast<int>((1.0F - icon_factor2) * icon_factor * rectMain.height()),
                   option.rect.top() + ((option.rect.bottom() - option.rect.top())/2 - icon_size/2),
                   icon_size, icon_size);

    painter->drawRect(rectIcon);
    //yue::qtcommon::SvgRender render;
    //render.paint(":/res/more.svg", painter, rectIcon);
    painter->drawPixmap(rectIcon, m_icoMore.pixmap(icon_size, icon_size));

    // create a rect covering the top half of the area
    QRect rectTop = rectMain;
    rectTop.setBottom(rectMain.top() + rectMain.height()/2);
    rectTop.setLeft(rectTop.left()+rectMain.height());
    // create a rect covering the bottom half of the area
    QRect rectBottomTime = rectMain;
    rectBottomTime.setTop(rectMain.top() + rectMain.height()/2);
    rectBottomTime.setLeft(rectBottomTime.left()+rectMain.height());
    // leave space on the RHS for the duration
    QRect rectBottom = rectBottomTime;
    rectBottom.setRight(rectBottom.right() - 6 * fm.averageCharWidth());

    painter->drawRect(rectMain.x()+2, rectMain.y()+2, rectMain.height() - 4, rectMain.height() - 4);
    int size = option.font.pixelSize();
    if (size > 0) {
        // TODO: revisit font sizing on windows
        font.setPixelSize( qMax(6, static_cast<int>(size * 1.1)));
        painter->setFont(font);
    }
    painter->drawText(rectTop, Qt::AlignVCenter|Qt::AlignLeft, song_title);

    if (size > 0) {
        font.setPixelSize( qMax(6, static_cast<int>(size * 0.8)));
        painter->setFont(font);
    }
    painter->drawText(rectBottom, Qt::AlignVCenter|Qt::AlignLeft, song_artist);

    painter->setFont(font);
    painter->drawText(rectBottomTime, Qt::AlignVCenter|Qt::AlignRight, song_duration);

    painter->restore();
}

QSize PlaylistDelegate::sizeHint(
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    Q_UNUSED(index);

    QFontMetrics fm(option.font);
    fm.height();
    int w = -1;
    int h = 3*fm.height();
    return QSize(w, h);
}

PlaylistView::PlaylistView(QWidget *parent)
  : QListView(parent)
  , m_gesture(this)
{
    m_model = new yue::qtcommon::PlaylistModel(this);
    m_delegate = new PlaylistDelegate(this);

    m_gesture.setViewport(this);
    m_gesture.setVScrollBar(this->verticalScrollBar());

    connect(&m_gesture, &yue::qtcommon::Gesture::tap,
            this, &PlaylistView::onTap);

    this->setModel(m_model);
    this->setItemDelegate(m_delegate);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::NoSelection);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    m_model->setPlaylistName(":current:");
    m_model->refresh();
}

void PlaylistView::refresh()
{
    m_model->refresh();
    qDebug() << "refresh";
}

void PlaylistView::move(int src, int dst)
{
    m_model->move(src, dst);
    m_model->refresh(); // TODO: playlist consistency

}

void PlaylistView::remove(int index)
{
    m_model->removeItem(index);
    m_model->refresh(); // TODO: playlist consistency
}

QueueMoreDialog::QueueMoreDialog(int row, int currentIndex, QWidget *parent)
    : QDialog(parent)
    , m_index(row)
{
    m_layoutCentral = new QVBoxLayout();
    setLayout(m_layoutCentral);

    m_btnPlay = new QPushButton("Play");
    m_btnPlayNext = new QPushButton("Play Next");
    m_btnRemove = new QPushButton("Remove");
    m_btnCancel = new QPushButton("Cancel");

    if (currentIndex == row) {
        m_btnPlay->setVisible(false);
        m_btnPlayNext->setVisible(false);
        m_btnRemove->setVisible(false);
    }

    m_layoutCentral->addWidget(m_btnPlay);
    m_layoutCentral->addWidget(m_btnPlayNext);
    m_layoutCentral->addWidget(m_btnRemove);
    m_layoutCentral->addWidget(m_btnCancel);

    connect(m_btnPlay, &QPushButton::clicked,
            this, &QueueMoreDialog::onPlay);
    connect(m_btnPlayNext, &QPushButton::clicked,
            this, &QueueMoreDialog::onPlayNext);
    connect(m_btnRemove, &QPushButton::clicked,
            this, &QueueMoreDialog::onRemove);
    connect(m_btnCancel, &QPushButton::clicked,
            this, &QueueMoreDialog::onCancel);
}

void QueueMoreDialog::onPlay()
{
    emit play(m_index);
    accept();
}

void QueueMoreDialog::onPlayNext()
{
    emit playNext(m_index);
    accept();
}

void QueueMoreDialog::onRemove()
{
    emit remove(m_index);
    accept();
}

void QueueMoreDialog::onCancel()
{
    reject();
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
    , m_ui(new UI::uiPageQueue(this))
{

    connect(m_ui->m_view, &PlaylistView::more,
            this, &PageQueue::onMore);

    auto inst = yue::bell::MediaCtrlBase::instance();

    connect(inst.data(), &yue::bell::MediaCtrlBase::playlistReset,
        this, &PageQueue::onPlaylistChanged);

    connect(inst.data(), &yue::bell::MediaCtrlBase::currentIndexChanged,
        this, &PageQueue::onCurrentIndexChanged);

    m_ui->m_view->setCurrentIndex(inst->currentIndex());

}

PageQueue::~PageQueue() {
}


void PageQueue::onMore(QModelIndex index)
{
    qDebug() << index.row();
    auto inst = yue::bell::MediaCtrlBase::instance();

    QueueMoreDialog dialog(index.row(), inst->currentIndex(), this);

    connect(&dialog, &QueueMoreDialog::play,
            this, &PageQueue::onPlay);

    connect(&dialog, &QueueMoreDialog::playNext,
            this, &PageQueue::onPlayNext);

    connect(&dialog, &QueueMoreDialog::remove,
            this, &PageQueue::onRemove);

    dialog.exec();
}

void PageQueue::onPlaylistChanged()
{
    m_ui->m_view->refresh();
}

void PageQueue::onCurrentIndexChanged(int index)
{
    m_ui->m_view->setCurrentIndex(index);
}


void PageQueue::onPlay(int index)
{
    auto inst = yue::bell::MediaCtrlBase::instance();
    inst->playIndex(index);
}

void PageQueue::onPlayNext(int index)
{
    auto inst = yue::bell::MediaCtrlBase::instance();

    //int dst = m_ui->m_view->currentIndex().row();
    int dst = inst->currentIndex();
    qDebug() << "move" << index << " to " << dst;
    m_ui->m_view->move(index, dst);

    inst->sync();
}

void PageQueue::onRemove(int index)
{
    auto inst = yue::bell::MediaCtrlBase::instance();
    m_ui->m_view->remove(index);

    inst->sync();
}
