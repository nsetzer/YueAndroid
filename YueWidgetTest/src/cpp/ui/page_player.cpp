

#include "ui/page_player.h"
#include "yue/bell/MediaCtrlBase.h"


namespace UI {

class uiPagePlayer
{

public:

    QVBoxLayout *m_layoutCentral;
    QHBoxLayout *m_layoutDisplay;

    QToolButton *m_btnMenu;
    QToolButton *m_btnPlayPause;
    QToolButton *m_btnNext;
    QProgressBar *m_barPosition;
    QLabel *m_lblSongInfo;


    uiPagePlayer(QWidget *parent = nullptr);
    ~uiPagePlayer();

};


uiPagePlayer::uiPagePlayer(QWidget *parent)
{
    m_layoutCentral = new QVBoxLayout();
    m_layoutDisplay = new QHBoxLayout();
    m_btnMenu = new QToolButton(parent);
    m_btnPlayPause = new QToolButton(parent);
    m_btnNext = new QToolButton(parent);
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
    connect(m_ui->m_btnPlayPause, &QToolButton::clicked,
        this, &PagePlayer::onPlayPauseClicked);

    connect(m_ui->m_btnNext, &QToolButton::clicked,
        this, &PagePlayer::onNextSongClicked);

    auto inst = yue::bell::MediaCtrlBase::instance();

    connect(inst.data(), &yue::bell::MediaCtrlBase::progressChanged,
            this, &PagePlayer::onProgressChanged);

    connect(inst.data(), &yue::bell::MediaCtrlBase::currentIndexChanged,
            this, &PagePlayer::onCurrentIndexChanged);

    onCurrentIndexChanged();

}

PagePlayer::~PagePlayer() {
}

void PagePlayer::onPlayPauseClicked(bool checked)
{
    Q_UNUSED(checked);
    auto inst = yue::bell::MediaCtrlBase::instance();
    inst->playpause();
}

void PagePlayer::onNextSongClicked(bool checked)
{
    Q_UNUSED(checked);
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
    Q_UNUSED(index);
    auto inst = yue::bell::MediaCtrlBase::instance();
    auto info = inst->currentSong();
    QString text = info.title() + " - " + info.artist();
    m_ui->m_lblSongInfo->setText(text);
}
