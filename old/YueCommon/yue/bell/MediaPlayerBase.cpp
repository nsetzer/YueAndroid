#include "yue/bell/MediaPlayerBase.h"

namespace yue {
namespace bell {


MediaPlayerBase::MediaPlayerBase(QObject *parent)
    : QObject(parent)
    , m_currentStatus(MediaPlayerBase::Status::Unknown)
    , m_currentState(MediaPlayerBase::State::Unknown)
{

}


} // bell
} // yue
