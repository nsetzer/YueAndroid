#include "yue/bell/MediaCtrlLocal.h"

#include "yue/bell/MediaPlayerQt.h"

namespace yue {
namespace bell {


MediaCtrlLocal::MediaCtrlLocal(QObject *parent/* = nullptr*/)
    : MediaCtrlBase(parent)
    , m_pBackend(new MediaCtrlBackend(this))
{
    connect(m_pBackend.data(),&MediaCtrlBackend::progressChanged,
            this,&MediaCtrlLocal::onProgressChanged);

    connect(m_pBackend.data(),&MediaCtrlBackend::currentIndexChanged,
            this,&MediaCtrlLocal::onCurrentIndexChanged);

    m_pBackend->load();
}

} // bell
} // yue
