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

    connect(m_pBackend.data(),&MediaCtrlBackend::stateChanged,
            this,&MediaCtrlLocal::onStateChanged);

    connect(m_pBackend.data(),&MediaCtrlBackend::statusChanged,
            this,&MediaCtrlLocal::onStatusChanged);
    try {
        m_pBackend->load();
    } catch (std::runtime_error& e) {
        qWarning() << "Error Loading Song: " << e.what();
    }

    loadCurrentSongInfo();
}

} // bell
} // yue
