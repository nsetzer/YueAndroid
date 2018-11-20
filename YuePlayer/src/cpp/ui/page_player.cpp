

#include "ui/page_player.h"
#include "yue/bell/MediaCtrlBase.h"
#include "yue/qtcommon/iconbutton.h"
#include "yue/qtcommon/mediaprogressbar.h"

namespace UI {

class uiPagePlayer
{

public:

    QVBoxLayout *m_layoutCentral;
    QHBoxLayout *m_layoutDisplay;

    yue::qtcommon::IconButton *m_btnMenu;
    yue::qtcommon::IconButton *m_btnPlayPause;
    yue::qtcommon::IconButton *m_btnNext;
    yue::qtcommon::MediaProgressBar *m_barInfo;

    uiPagePlayer(QWidget *parent = nullptr);
    ~uiPagePlayer();

};


uiPagePlayer::uiPagePlayer(QWidget *parent)
{

    m_layoutCentral = new QVBoxLayout();
    m_layoutCentral->setContentsMargins(0, 0, 0, 0);

    m_layoutDisplay = new QHBoxLayout();
    m_layoutDisplay->setContentsMargins(0, 0, 0, 0);

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
    m_barInfo = new yue::qtcommon::MediaProgressBar(parent);

    m_layoutDisplay->addWidget(m_btnMenu);
    m_layoutDisplay->addWidget(m_btnPlayPause);
    m_layoutDisplay->addWidget(m_btnNext);
    //m_layoutDisplay->addWidget(m_barPosition);

    parent->setLayout(m_layoutCentral);
    m_layoutCentral->addSpacing(8);
    m_layoutCentral->addLayout(m_layoutDisplay);

    m_layoutCentral->addWidget(m_barInfo);
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

    connect(m_ui->m_barInfo, &yue::qtcommon::MediaProgressBar::positionChanged,
            this, &PagePlayer::onPositionChanged);

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
    m_ui->m_barInfo->setPosition(progress);
}

void PagePlayer::onCurrentIndexChanged(int index)
{
    qDebug() << "new index" << index;
    Q_UNUSED(index);
    auto inst = yue::bell::MediaCtrlBase::instance();
    auto info = inst->currentSong();
    QString text = info.title() + " - " + info.artist();
    m_ui->m_barInfo->setText(text);
    //m_ui->m_barInfo->setLength(info.length)
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

void PagePlayer::onPositionChanged(float pos)
{
    auto inst = yue::bell::MediaCtrlBase::instance();
    inst->setProgress(pos);
}
