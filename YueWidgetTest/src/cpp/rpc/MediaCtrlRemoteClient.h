#ifndef RPC_MEDIA_CTRL_REMOTE_CLIENT_H
#define RPC_MEDIA_CTRL_REMOTE_CLIENT_H

#include "rpc/MediaCtrlRemoteServer.h"
#include "yue/bell/MediaCtrlBase.h"
#include "rep_MediaControl_replica.h"

class MediaCtrlRemoteClient : public yue::bell::MediaCtrlBase
{
    Q_OBJECT

public:
    MediaCtrlRemoteClient();

    Q_INVOKABLE virtual void tts() override;

signals:
    //void progressChanged(float progress);
    //void currentIndexChanged(int index);
    //void statusChanged(yue::bell::MediaPlayerBase::Status status);
    //void stateChanged(yue::bell::MediaPlayerBase::State state);
    //void syncUpdate(bool running, int ndirs, int nfiles, int nsongs);

public slots:

    virtual void load() override;
    virtual void loadIndex(int index) override;
    virtual void playIndex(int index) override;
    virtual void playpause() override;
    virtual void next() override;
    virtual void prev() override;
    virtual void sync() override;
    virtual void setVolume(float volume) override;
    virtual void setProgress(float progress) override;


private slots:
    void onServiceProgressChanged(float progress);
    void onServiceCurrentIndexChanged(int index);
    void onServiceStatusChanged(int index);
    void onServiceStateChanged(int index);
    void onServiceSyncUpdate(bool running, int ndirs, int nfiles, int nsongs);

private:
    QRemoteObjectNode m_repNode;
    QSharedPointer<MediaControlReplica> m_rep;

};

#endif // MEDIACTRLREMOTE_H
