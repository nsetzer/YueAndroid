#include "yue/bell/MediaPlayerBase.h"

namespace yue {
namespace bell {


MediaPlayerBase::MediaPlayerBase(QObject *parent)
    : QObject(parent)
    , m_currentStatus(MediaPlayerBase::Status::Unknown)
    , m_currentState(MediaPlayerBase::State::Unknown)
{

}

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


QString MediaPlayerBase::statusToString(MediaPlayerBase::Status status)
{
    switch (status) {
    case MediaPlayerBase::Status::Error:
        return "Error";
    case MediaPlayerBase::Status::Loading:
        return "Loading";
    case MediaPlayerBase::Status::Ready:
        return "Ready";
    case MediaPlayerBase::Status::Ended:
        return "Ended";
    default:
        return "Unknown";
    }
}

QString MediaPlayerBase::stateToString(MediaPlayerBase::State state)
{
    switch (state) {
    case MediaPlayerBase::State::Playing:
        return "Playing";
    case MediaPlayerBase::State::Paused:
        return "Paused";
    default:
        return "Unknown";
    }
}

} // bell
} // yue

QDebug& operator << (QDebug& os, const yue::bell::MediaPlayerBase::State& state);
QDebug& operator << (QDebug& os, const yue::bell::MediaPlayerBase::Status& status);

QDebug& operator << (QDebug& os, const yue::bell::MediaPlayerBase::State& state)
{
    os << yue::bell::MediaPlayerBase::stateToString(state);
    return os;
}

QDebug& operator << (QDebug& os, const yue::bell::MediaPlayerBase::Status& status)
{
    os << yue::bell::MediaPlayerBase::statusToString(status);
    return os;
}

/*
std::ostream& operator << (std::ostream& os, yue::bell::MediaPlayerBase::State const & state)
{
    os << yue::bell::MediaPlayerBase::stateToString(state).toStdString();
    return os;
}

std::ostream& operator << (std::ostream& os, yue::bell::MediaPlayerBase::Status const & status)
{
    os << yue::bell::MediaPlayerBase::statusToString(status).toStdString();
    return os;
}
*/
