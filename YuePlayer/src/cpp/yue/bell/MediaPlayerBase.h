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
        Unknown=0,
        Error=1,    // error is preventing playback
        Loading=2,  // media is not ready to play
        Ready=3,    // play() will begin playback
        Ended=4,    // media has finished playing
    };

    enum class State {
        Unknown=0,
        Playing=1,
        Paused=2,
    };


    explicit MediaPlayerBase(QObject *parent = nullptr);

    virtual float duration() const = 0;
    virtual float position() const = 0;
    virtual void setPosition(float seconds) = 0;

    static QString statusToString(Status status);
    static QString stateToString(State state);

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
    virtual void stop()=0;

protected:
    Status m_currentStatus;
    State m_currentState;

public:
    friend std::ostream& operator << (std::ostream& os, State const& type) {
        os << yue::bell::MediaPlayerBase::stateToString(type).toStdString();
        return os;
    }
    friend std::ostream& operator << (std::ostream& os, Status const& type) {
        os << yue::bell::MediaPlayerBase::statusToString(type).toStdString();
        return os;
    }
};



} // bell
} // yue

QDebug& operator << (QDebug& os, const yue::bell::MediaPlayerBase::State& state);
QDebug& operator << (QDebug& os, const yue::bell::MediaPlayerBase::Status& status);

//std::ostream& operator << (std::ostream& os, yue::bell::MediaPlayerBase::State const& type);
//std::ostream& operator << (std::ostream& os, yue::bell::MediaPlayerBase::Status const& type);


// declare outside the namespace, so that it will build on linux
Q_DECLARE_METATYPE(yue::bell::MediaPlayerBase::State)
Q_DECLARE_METATYPE(yue::bell::MediaPlayerBase::Status)

#endif // MEDIAPLAYER_H
