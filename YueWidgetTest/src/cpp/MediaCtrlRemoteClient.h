#ifndef MEDIACTRLREMOTE_H
#define MEDIACTRLREMOTE_H

#include "MediaCtrlRemoteServer.h"
//#include "yue/bell/MediaCtrlBase.h"
#include "rep_MediaControl_replica.h"

class MediaCtrlRemoteClient : public QObject //: public yue::bell::MediaCtrlBase
{
    Q_OBJECT

public:
    MediaCtrlRemoteClient();

    Q_INVOKABLE virtual void tts();

signals:
    //void progressChanged(float progress);
    //void currentIndexChanged(int index);
    //void statusChanged(Status status);
    //void stateChanged(State state);

public slots:

    virtual void load();
    virtual void loadIndex(int index);
    virtual void playIndex(int index);
    virtual void playpause();
    virtual void next();
    virtual void prev();
    virtual void setVolume(float volume);
    virtual void setProgress(float progress);


private slots:
    void onServiceProgressChanged(float progress);
    void onServiceCurrentIndexChanged(int index);
    void onServiceStatusChanged(int index);
    void onServiceStateChanged(int index);
private:
    QRemoteObjectNode m_repNode;
    QSharedPointer<MediaControlReplica> m_rep;

};

#endif // MEDIACTRLREMOTE_H
