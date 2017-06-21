#include "MediaCtrlBase.h"

namespace yue {
namespace bell {


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
