#ifndef MEDIACONTROLBACKEND_H
#define MEDIACONTROLBACKEND_H

#include <QSharedPointer>
#include <QByteArray>
#include "rep_MediaControl_source.h"
#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
#include "yue/bell/playlist.hpp"
#include "yue/bell/MediaCtrlBackend.h"


class MediaCtrlRemoteServer : public MediaControlSource
{

    static QSharedPointer<MediaCtrlRemoteServer> m_instance;

public:
    MediaCtrlRemoteServer();
    virtual ~MediaCtrlRemoteServer() = default;

    static QSharedPointer<MediaCtrlRemoteServer> create() {
        m_instance = QSharedPointer<MediaCtrlRemoteServer>(new MediaCtrlRemoteServer());
        return m_instance;
    }

    static QSharedPointer<MediaCtrlRemoteServer> instance() {
        return m_instance;
    }

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

    virtual void stop() { m_pBackend->stop(); }

private slots:
    void onProgressChanged(float progress);
    void onCurrentIndexChanged(int index);
    void onStatusChanged(yue::bell::MediaPlayerBase::Status status);
    void onStateChanged(yue::bell::MediaPlayerBase::State state);

private:
    QScopedPointer<yue::bell::MediaCtrlBackend> m_pBackend;

    int m_index;
    yue::bell::MediaPlayerBase::Status m_status;
    yue::bell::MediaPlayerBase::State m_state;

    void sendNotification();

    QByteArray m_current_art;
    QString m_title;
    QString m_message;

};

#endif // MEDIACONTROLBACKEND_H
