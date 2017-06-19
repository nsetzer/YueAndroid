#include "yue/bell/MediaPlayerQt.h"

namespace yue {
namespace bell {


MediaPlayerQt::MediaPlayerQt(QObject *parent)
    : MediaPlayerBase(parent)
    , m_player(this)
    , m_duration(0)
    , m_progress(0)
    , m_progress_resolution(240)

{
    connect(&m_player,&QMediaPlayer::durationChanged,
            this,&MediaPlayerQt::onDurationChanged);
    connect(&m_player,&QMediaPlayer::positionChanged,
            this,&MediaPlayerQt::onPositionChanged);
    setVolume(0.5);
}

void MediaPlayerQt::load(QString path)
{
    QMediaContent content(QUrl::fromLocalFile(path));
    m_player.setMedia(content);
}

void MediaPlayerQt::playpause()
{
    if (m_player.state() == QMediaPlayer::PlayingState) {
        m_player.pause();
    } else {
        m_player.play();
    }
}

void MediaPlayerQt::setVolume(float volume)
{
    int v = qMin(100,qMax(0,static_cast<int>(100*volume)));
    m_player.setVolume(v);
}

void MediaPlayerQt::onDurationChanged(qint64 duration)
{
    // convert from milliseconds to seconds as float
    m_duration = duration/1000.0;
}

void MediaPlayerQt::onPositionChanged(qint64 position)
{
    float seconds = position/1000.0;
    float f = seconds / m_duration;
    int p = static_cast<int>(m_progress_resolution * f);
    // limit the total number of signals that are broadcast
    if (p != m_progress) {
        m_progress = p;
        emit progressChanged(f);
    }
}

} // bell
} // yue
