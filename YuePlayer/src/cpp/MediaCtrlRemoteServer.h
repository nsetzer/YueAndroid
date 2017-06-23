#ifndef MEDIACONTROLBACKEND_H
#define MEDIACONTROLBACKEND_H

#include <QScopedPointer>
#include "rep_MediaControl_source.h"
#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
#include "yue/bell/playlist.hpp"
#include "yue/bell/MediaCtrlBackend.h"


class MediaCtrlRemoteServer : public MediaControlSource
{

public:
    MediaCtrlRemoteServer();
    virtual ~MediaCtrlRemoteServer() = default;

signals:
    //void progressChanged(float progress);
    //void currentIndexChanged(int index);
    //void stateChanged(State state)
    //void statusChanged(Status Status)

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
    void onStatusChanged(yue::bell::MediaPlayerBase::Status status) {
        emit statusChanged(static_cast<int>(status));
    }
    void onStateChanged(yue::bell::MediaPlayerBase::State state) {
        emit stateChanged(static_cast<int>(state));
    }

private:
    QScopedPointer<yue::bell::MediaCtrlBackend> m_pBackend;

};

#endif // MEDIACONTROLBACKEND_H
