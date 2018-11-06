

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

    connect(m_ui->m_btnPlayPause, &QToolButton::clicked,
        this, &PagePlayer::onPlayPauseClicked);
}

PagePlayer::~PagePlayer() {
}

void PagePlayer::onPlayPauseClicked(bool checked)
{
    auto inst = yue::bell::MediaCtrlBase::instance();
    inst->loadIndex(0);
    auto info = inst->currentSong();
    inst->playpause();
}
