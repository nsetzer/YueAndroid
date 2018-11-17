#ifndef YUE_BELL_MEDIACTRLBACKEND_H
#define YUE_BELL_MEDIACTRLBACKEND_H

#include "yue/global.h"
#include "yue/bell/MediaPlayerBase.h"
#include "yue/bell/MediaPlayerQt.h"
#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
#include "yue/bell/playlist.hpp"
#include "yue/bell/scanner.h"
/*
 * The backend holds the implementation for media events.
 * Signals and slots are used to transfer state
 * A local implementation is used for single-process applications
 * A remote implementation is used to separate the client/server
 * into separate processes.
 *
 *  Adding a new signal / slot
 *  - implement a slot in MediaCtrlBackend
 *  - add a signal to MediaCtrlBackend
 *  - add a virtual public slot to MediaCtrlBase
 *  - add a copy of the signal to MediaCtrlBase
 *  - in MediaCtrlLocal add a public slot to call the backend slot
 *  - in MediaCtrlLocal adda private slot to receive the backend signal,
 *    and then emit the base signal
 *  - in MediaControl.rep, add the signal and slots
 *  - the class MediaControlSource is built from the rep by running qmake
 *  - implement MediaCtrlRemoteServer signals and slots similar to local
 *  - implement MediaCtrlRemoteClient to call the server slots
 *    and listen to server signals.
 */
namespace yue {
namespace bell {


class YUECOMMON_EXPORT MediaCtrlBackend : public QObject
{
    Q_OBJECT

public:
    MediaCtrlBackend(QObject *parent = nullptr);
    virtual ~MediaCtrlBackend() = default;

signals:
    void progressChanged(float progress);
    void currentIndexChanged(int index);
    void statusChanged(yue::bell::MediaPlayerBase::Status status);
    void stateChanged(yue::bell::MediaPlayerBase::State state);
    void scanUpdate(bool running, int ndirs, int nfiles, int nsongs);

public slots:

    void load();
    void loadIndex(int index);
    void playIndex(int index);
    void playpause();
    void next();
    void prev();
    void setVolume(float volume);
    void setProgress(float progress);
    void sync();
    void stop();

    void scanStart(QString path);
    void scanStop();
    void scanStatus();

private slots:
    void onProgressChanged(float progress);
    void onCurrentIndexChanged(int index);

    void onStatusChanged(MediaPlayerBase::Status status);
    void onStateChanged(MediaPlayerBase::State state);

    void onSongEnded();

    void load(Database::uid_t uid);

private:
    QScopedPointer<MediaPlayerBase> m_pPlayer;
    Scanner m_scanner;

};

} // bell
} // yue


#endif // MEDIACTRLBACKEND_H
