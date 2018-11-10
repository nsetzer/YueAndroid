

#include "ui/page_player.h"
#include "yue/bell/MediaCtrlBase.h"
#include "yue/qtcommon/iconbutton.h"


namespace UI {

class uiPagePlayer
{

public:

    QVBoxLayout *m_layoutCentral;
    QHBoxLayout *m_layoutDisplay;

    yue::qtcommon::IconButton *m_btnMenu;
    yue::qtcommon::IconButton *m_btnPlayPause;
    yue::qtcommon::IconButton *m_btnNext;
    QProgressBar *m_barPosition;
    QLabel *m_lblSongInfo;


    uiPagePlayer(QWidget *parent = nullptr);
    ~uiPagePlayer();

};


uiPagePlayer::uiPagePlayer(QWidget *parent)
{
    m_layoutCentral = new QVBoxLayout();
    m_layoutDisplay = new QHBoxLayout();
    m_btnMenu = new yue::qtcommon::IconButton(
                yue::qtcommon::IconButton::IconSize::LARGE,
                QIcon(":/res/home.svg"),
                parent);
    m_btnPlayPause = new yue::qtcommon::IconButton(
                yue::qtcommon::IconButton::IconSize::LARGE,
                QIcon(":/res/media_play.svg"),
                parent);
    m_btnNext = new yue::qtcommon::IconButton(
                yue::qtcommon::IconButton::IconSize::LARGE,
                QIcon(":/res/media_next.svg"),
                parent);
    m_barPosition = new QProgressBar(parent);
    m_barPosition->setTextVisible(false);
    m_barPosition->setRange(0, 100);
    m_lblSongInfo = new QLabel("TITLE - ARTIST", parent);
    m_lblSongInfo->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    m_layoutDisplay->addWidget(m_btnMenu);
    m_layoutDisplay->addWidget(m_btnPlayPause);
    m_layoutDisplay->addWidget(m_btnNext);
    m_layoutDisplay->addWidget(m_barPosition);

    parent->setLayout(m_layoutCentral);
    m_layoutCentral->addLayout(m_layoutDisplay);
    m_layoutCentral->addWidget(m_lblSongInfo);
}

uiPagePlayer::~uiPagePlayer()
{

}

} // namespace ui


PagePlayer::PagePlayer(QWidget *parent)
    : QWidget(parent)
    , m_ui(new UI::uiPagePlayer(this))
    , m_status(yue::bell::MediaPlayerBase::Status::Unknown)
{

    /*
    void playlistReset();

    // signals emitted by the server
    void progressChanged(float progress);
    void currentIndexChanged(int index);
    void statusChanged(MediaPlayerBase::Status status);
    void stateChanged(MediaPlayerBase::State state);

    void doPlaySong(int uid);
    void doSetCurrentPlaylist(QList<yue::bell::Database::uid_t> lst, bool autoplay);
    */
    connect(m_ui->m_btnPlayPause, &yue::qtcommon::IconButton::clicked,
        this, &PagePlayer::onPlayPauseClicked);

    connect(m_ui->m_btnNext, &yue::qtcommon::IconButton::clicked,
        this, &PagePlayer::onNextSongClicked);

    auto inst = yue::bell::MediaCtrlBase::instance();

    connect(inst.data(), &yue::bell::MediaCtrlBase::progressChanged,
            this, &PagePlayer::onProgressChanged);

    connect(inst.data(), &yue::bell::MediaCtrlBase::currentIndexChanged,
            this, &PagePlayer::onCurrentIndexChanged);

    connect(inst.data(), &yue::bell::MediaCtrlBase::statusChanged,
            this, &PagePlayer::onStatusChanged);

    connect(inst.data(), &yue::bell::MediaCtrlBase::stateChanged,
            this, &PagePlayer::onStateChanged);

    onCurrentIndexChanged();

}

PagePlayer::~PagePlayer() {
}

void PagePlayer::onPlayPauseClicked()
{
    auto inst = yue::bell::MediaCtrlBase::instance();
    inst->playpause();
}

void PagePlayer::onNextSongClicked()
{
    auto inst = yue::bell::MediaCtrlBase::instance();
    inst->next();
}

void PagePlayer::onProgressChanged(float progress)
{
    m_ui->m_barPosition->setValue(
                static_cast<int>(m_ui->m_barPosition->maximum()*progress));
}

void PagePlayer::onCurrentIndexChanged(int index)
{
    qDebug() << "new index" << index;
    Q_UNUSED(index);
    auto inst = yue::bell::MediaCtrlBase::instance();
    auto info = inst->currentSong();
    QString text = info.title() + " - " + info.artist();
    m_ui->m_lblSongInfo->setText(text);
}

void PagePlayer::onStatusChanged(yue::bell::MediaPlayerBase::Status status)
{
    m_status = status;
    switch(status) {
    case yue::bell::MediaPlayerBase::Status::Ready:
        break;
    case yue::bell::MediaPlayerBase::Status::Loading:
        break;
    case yue::bell::MediaPlayerBase::Status::Ended:
        break;
    case yue::bell::MediaPlayerBase::Status::Error:
        m_ui->m_btnPlayPause->setIcon(QIcon(":/res/media_error.svg"));
        break;
    default:
        break;
    }
}
void PagePlayer::onStateChanged(yue::bell::MediaPlayerBase::State state)
{
    if (m_status == yue::bell::MediaPlayerBase::Status::Error) {
        m_ui->m_btnPlayPause->setIcon(QIcon(":/res/media_error.svg"));
        return;
    }

    switch(state) {
    case yue::bell::MediaPlayerBase::State::Playing:
        m_ui->m_btnPlayPause->setIcon(QIcon(":/res/media_pause.svg"));
        break;
    case yue::bell::MediaPlayerBase::State::Paused:
        m_ui->m_btnPlayPause->setIcon(QIcon(":/res/media_play.svg"));
        break;
    default:
        break;
    }
}

