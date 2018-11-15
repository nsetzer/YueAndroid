#ifndef YUE_BELL_MEDIAPLAYERQT_H
#define YUE_BELL_MEDIAPLAYERQT_H

#include <QUrl>
#include <QMediaPlayer>
#include <QMediaContent>
#include <QObject>
#include <QFileInfo>
#include "yue/global.h"
#include "yue/bell/MediaPlayerBase.h"

namespace yue {
namespace bell {


class MediaPlayerQt : public MediaPlayerBase
{
    Q_OBJECT

    QMediaPlayer m_player;
    float m_duration;
    int m_progress;
    int m_progress_resolution;
public:
    explicit MediaPlayerQt(QObject *parent = nullptr);

    virtual float duration() const override { return m_duration; }
    virtual float position() const override { return m_player.position()/1000.0; }
    virtual void setPosition(float seconds) override { m_player.setPosition(seconds*1000.0); }

signals:

    //void progressChanged(float progress); // as float, 0.0 to 1.0

public slots:

    virtual void load(QString path) override;
    virtual void play() override;
    virtual void pause() override;
    virtual void playpause() override;
    //virtual void next();
    //virtual void prev();
    virtual void setVolume(float volume) override;
    virtual void stop() override;

private slots:
    void onDurationChanged(qint64 duration);
    void onPositionChanged(qint64 position);
    void onStatusChanged(QMediaPlayer::MediaStatus status);
    void onStateChanged(QMediaPlayer::State status);

};

} // bell
} // yue

#endif // MEDIAPLAYER_H
