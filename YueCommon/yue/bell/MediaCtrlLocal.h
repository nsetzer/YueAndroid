#ifndef YUE_BELL_MEDIACTRLLOCAL_H
#define YUE_BELL_MEDIACTRLLOCAL_H


#include "yue/global.h"
#include "yue/bell/MediaCtrlBase.h"
#include "yue/bell/MediaCtrlBackend.h"
#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
#include "yue/bell/playlist.hpp"

namespace yue {
namespace bell {


class YUECOMMON_EXPORT MediaCtrlLocal : public MediaCtrlBase
{

public:
    MediaCtrlLocal(QObject *parent = nullptr);
    virtual ~MediaCtrlLocal() = default;

signals:
    //void progressChanged(float progress);
    //void currentIndexChanged(int index);
    //void statusChanged(Status status);
    //void stateChanged(State state);

public slots:

    virtual void load() { m_pBackend->load(); }
    virtual void loadIndex(int index) { m_pBackend->loadIndex(index); }
    virtual void playIndex(int index) { m_pBackend->playIndex(index); }
    virtual void playpause() { m_pBackend->playpause(); }
    virtual void next() { m_pBackend->next(); }
    virtual void prev() { m_pBackend->prev(); }
    virtual void setVolume(float volume) { m_pBackend->setVolume(volume); }
    virtual void setProgress(float progress) { m_pBackend->setProgress(progress); }

private slots:
    void onProgressChanged(float progress) { emit progressChanged(progress); }
    void onCurrentIndexChanged(int index) { emit currentIndexChanged(index); }
    void onStatusChanged(MediaPlayerBase::Status status) { emit statusChanged(status); }
    void onStateChanged(yue::bell::MediaPlayerBase::State state) { emit stateChanged(state); }

private:
    QScopedPointer<MediaCtrlBackend> m_pBackend;
};

} // bell
} // yue

#endif // MEDIACTRLLOCAL_H
