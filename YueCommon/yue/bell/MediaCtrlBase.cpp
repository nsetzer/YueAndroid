#include "MediaCtrlBase.h"

namespace yue {
namespace bell {

QSharedPointer<yue::bell::MediaCtrlBase> MediaCtrlBase::m_instance = nullptr;

MediaCtrlBase::MediaCtrlBase(QObject *parent) : QObject(parent)
{

}

size_t MediaCtrlBase::currentIndex()
{
    auto pl = PlaylistManager::instance()->openCurrent();
    auto pair = pl->current();
    return static_cast<int>(pair.second);
}

} // bell
} // yue
