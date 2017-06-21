#ifndef YUE_BELL_MEDIAPLAYERBASE_H
#define YUE_BELL_MEDIAPLAYERBASE_H

#include <QObject>
#include "yue/global.h"

namespace yue {
namespace bell {

class YUECOMMON_EXPORT MediaPlayerBase : public QObject
{
    Q_OBJECT

public:
    //
    // after loading move to ready state
    // pausing the media will return to ready state
    enum class Status {
        Unknown,
        Error,    // error is preventing playback
        Loading,  // media is not ready to play
        Ready,    // play() will begin playback
        Ended,    // media has finished playing
    };

    enum class State {
        Unknown,
        Playing,
        Paused,
    };


    explicit MediaPlayerBase(QObject *parent = nullptr);

    virtual float duration() const = 0;
    virtual float position() const = 0;
    virtual void setPosition(float seconds) = 0;

signals:

    void progressChanged(float progress); // as float, 0.0 to 1.0
    void statusChanged(Status status);
    void stateChanged(State state);

public slots:

    virtual void load(QString path)=0;
    virtual void play()=0;
    virtual void pause()=0;
    virtual void playpause()=0;
    //virtual void next()=0;
    //virtual void prev()=0;
    virtual void setVolume(float volume) = 0;

protected:
    Status m_currentStatus;
    State m_currentState;

};

} // bell
} // yue

#endif // MEDIAPLAYER_H
