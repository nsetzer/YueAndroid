#include "MediaCtrlRemoteServer.h"

MediaCtrlRemoteServer::MediaCtrlRemoteServer()
    : MediaControlSource()
    , m_pBackend(new yue::bell::MediaCtrlBackend(this))
{
    connect(m_pBackend.data(),&yue::bell::MediaCtrlBackend::progressChanged,
            this,&MediaCtrlRemoteServer::onProgressChanged);

    connect(m_pBackend.data(),&yue::bell::MediaCtrlBackend::currentIndexChanged,
            this,&MediaCtrlRemoteServer::onCurrentIndexChanged);

    connect(m_pBackend.data(),&yue::bell::MediaCtrlBackend::stateChanged,
            this,&MediaCtrlRemoteServer::onStateChanged);

    connect(m_pBackend.data(),&yue::bell::MediaCtrlBackend::statusChanged,
            this,&MediaCtrlRemoteServer::onStatusChanged);

    try {
        m_pBackend->load();
    } catch (std::runtime_error& e) {
        qWarning() << "Error Loading Song: " << e.what();
    }
}


