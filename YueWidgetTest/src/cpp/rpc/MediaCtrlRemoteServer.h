#ifndef RPC_MEDIA_CTRL_REMOTE_SERVER_H
#define RPC_MEDIA_CTRL_REMOTE_SERVER_H

#include <QSharedPointer>
#include <QByteArray>
#include "rep_MediaControl_source.h"
#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
#include "yue/bell/playlist.hpp"
#include "yue/bell/MediaCtrlBackend.h"


class MediaCtrlRemoteServer : public MediaControlSource
{
    Q_OBJECT
    static QSharedPointer<MediaCtrlRemoteServer> m_instance;

public:
    MediaCtrlRemoteServer();

    static QSharedPointer<MediaCtrlRemoteServer> create();
    static QSharedPointer<MediaCtrlRemoteServer> instance();
signals:
    // NOTE: signals provided for easy reference
    // DO NOT define signals here, but in the rep
    // signals defiend here will override the remote object signals
    // and prevent everything from working
    //void progressChanged(float progress);
    //void currentIndexChanged(int index);
    //void stateChanged(int state);
    //void statusChanged(int Status);
    //void syncUpdate(bool running, int ndirs, int nfiles, int nsongs);


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

    virtual void stop() { m_pBackend->stop(); }

private slots:
    void onProgressChanged(float progress);
    void onCurrentIndexChanged(int index);
    void onStatusChanged(yue::bell::MediaPlayerBase::Status status);
    void onStateChanged(yue::bell::MediaPlayerBase::State state);
    void onSyncUpdate(bool running, int ndirs, int nfiles, int nsongs);

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
