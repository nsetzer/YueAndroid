#ifndef YUE_BELL_MEDIACTRLBACKEND_H
#define YUE_BELL_MEDIACTRLBACKEND_H

#include "yue/global.h"
#include "yue/bell/MediaPlayerBase.h"
#include "yue/bell/MediaPlayerQt.h"
#include "yue/bell/database.hpp"
#include "yue/bell/library.hpp"
#include "yue/bell/playlist.hpp"

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

private slots:
    void onProgressChanged(float progress);
    void onCurrentIndexChanged(int index);

    void onStatusChanged(MediaPlayerBase::Status status);
    void onStateChanged(MediaPlayerBase::State state);

    void onSongEnded();

    void load(Database::uid_t uid);

private:
    QScopedPointer<MediaPlayerBase> m_pPlayer;
};

} // bell
} // yue


#endif // MEDIACTRLBACKEND_H
