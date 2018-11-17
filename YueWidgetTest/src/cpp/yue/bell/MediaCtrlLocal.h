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

signals:
    //void progressChanged(float progress);
    //void currentIndexChanged(int index);
    //void statusChanged(MediaPlayerBase::Status status);
    //void stateChanged(MediaPlayerBase::State state);
    //void scanUpdate(bool running, int ndirs, int nfiles, int nsongs);

public slots:

    virtual void load() override { m_pBackend->load(); }
    virtual void loadIndex(int index) override { m_pBackend->loadIndex(index); }
    virtual void playIndex(int index) override { m_pBackend->playIndex(index); }
    virtual void playpause() override { m_pBackend->playpause(); }
    virtual void next() override { m_pBackend->next(); }
    virtual void prev() override { m_pBackend->prev(); }
    virtual void setVolume(float volume) override { m_pBackend->setVolume(volume); }
    virtual void setProgress(float progress) override { m_pBackend->setProgress(progress); }
    virtual void sync() override { m_pBackend->sync(); }

    virtual void scanStart(QString path) override { m_pBackend->scanStart(path); }
    virtual void scanStop() override { m_pBackend->scanStop(); }
    virtual void scanStatus() override { m_pBackend->scanStatus(); }


private slots:
    void onProgressChanged(float progress) {
        m_progress = progress;
        emit progressChanged(progress);
    }
    void onCurrentIndexChanged(int index) {
        loadCurrentSongInfo();
        emit currentIndexChanged(index);
    }
    void onStatusChanged(MediaPlayerBase::Status status) {
        m_status = status;
        emit statusChanged(status);
    }
    void onStateChanged(yue::bell::MediaPlayerBase::State state) {
        m_state = state;
        emit stateChanged(state);
    }
    void onScanUpdate(bool running, int ndirs, int nfiles, int nsongs) {
        emit scanUpdate(running, ndirs, nfiles, nsongs);
    }

private:
    QScopedPointer<MediaCtrlBackend> m_pBackend;
};

} // bell
} // yue

#endif // MEDIACTRLLOCAL_H
