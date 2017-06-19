#ifndef YUE_BELL_MEDIAPLAYERQT_H
#define YUE_BELL_MEDIAPLAYERQT_H

#include <QUrl>
#include <QMediaPlayer>
#include <QMediaContent>
#include <QObject>
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

signals:

    //void progressChanged(float progress); // as float, 0.0 to 1.0

public slots:

    virtual void load(QString path);
    virtual void playpause();
    //virtual void next();
    //virtual void prev();
    virtual void setVolume(float volume);

private slots:
    void onDurationChanged(qint64 duration);
    void onPositionChanged(qint64 position);

};

} // bell
} // yue

#endif // MEDIAPLAYER_H
